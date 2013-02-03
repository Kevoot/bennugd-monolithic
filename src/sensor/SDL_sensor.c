/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2012 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "SDL_config.h"

/* This is the sensor API for Simple DirectMedia Layer */

#include "SDL_events.h"
#include "SDL_syssensor.h"
#include "SDL_assert.h"

#if !SDL_EVENTS_DISABLED
#include "../events/SDL_events_c.h"
#endif

static SDL_Sensor *SDL_sensors = NULL;
static SDL_Sensor *SDL_updating_sensor = NULL;

int
SDL_SensorInit(void)
{
    int status;

    status = SDL_SYS_SensorInit();
    if (status >= 0) {
      status = 0;
    }
    return (status);
}

/*
 * Count the number of sensors attached to the system
 */
int
SDL_NumSensors(void)
{
    return SDL_SYS_NumSensors();
}

/*
 * Get the implementation dependent name of a sensor
 */
const char *
SDL_SensorNameForIndex(int device_index)
{
    if ((device_index < 0) || (device_index >= SDL_NumSensors())) {
        SDL_SetError("There are %d sensors available", SDL_NumSensors());
        return (NULL);
    }
    return (SDL_SYS_SensorNameForIndex(device_index));
}

/*
 * Open a sensor for use - the index passed as an argument refers to
 * the N'th sensor on the system.  This index is the value which will
 * identify this sensor in future sensor events.
 *
 * This function returns a sensor identifier, or NULL if an error occurred.
 */
SDL_Sensor *
SDL_SensorOpen(int device_index)
{
    SDL_Sensor *sensor;
    SDL_Sensor *sensorlist;
    const char *sensorname = NULL;

    if ((device_index < 0) || (device_index >= SDL_NumSensors())) {
        SDL_SetError("There are %d sensors available", SDL_NumSensors());
        return (NULL);
    }

    sensorlist = SDL_sensors;
    /* If the sensor is already open, return it
    * it is important that we have a single sensor * for each instance id
    */
    while ( sensorlist )
    {
        if ( SDL_SYS_GetInstanceIdOfDeviceIndex(device_index) == sensorlist->instance_id ) {
                sensor = sensorlist;
                ++sensor->ref_count;
                return (sensor);
        }
        sensorlist = sensorlist->next;
    }

    /* Create and initialize the sensor */
    sensor = (SDL_Sensor *) SDL_malloc((sizeof *sensor));
    if (sensor == NULL) {
        SDL_OutOfMemory();
        return NULL;
    }

    SDL_memset(sensor, 0, (sizeof *sensor));
    if (SDL_SYS_sensorOpen(sensor, device_index) < 0) {
        SDL_free(sensor);
        return NULL;
    }

    sensorname = SDL_SYS_sensorNameForDeviceIndex( device_index );
    if ( sensorname )
        sensor->name = SDL_strdup( sensorname );
    else
        sensor->name = NULL;

    if (sensor->naxes > 0) {
        sensor->axes = (Sint16 *) SDL_malloc
            (sensor->naxes * sizeof(Sint16));
        sensor->resolutions = (Sint16 *) SDL_malloc
            (sensor->naxes * sizeof(Sint16));
    }
    if ( (sensor->naxes > 0) && (!sensor->axes || !sensor->resolutions) ) {
        SDL_OutOfMemory();
        SDL_SensorClose(sensor);
        return NULL;
    }
    if (sensor->axes) {
        SDL_memset(sensor->axes, 0, sensor->naxes * sizeof(Sint16));
    }
    if (sensor->resolutions) {
        SDL_memset(sensor->resolutions, 0, sensor->resolutions * sizeof(Sint16));
    }

    /* Add sensor to list */
    ++sensor->ref_count;
    /* Link the sensor in the list */
    sensor->next = SDL_Sensors;
    SDL_Sensors = sensor;

    SDL_SYS_sensorUpdate( sensor );

    return (sensor);
}


/*
 * Checks to make sure the sensor is valid.
 */
int
SDL_PrivatesensorValid(SDL_Sensor * sensor)
{
    int valid;

    if ( sensor == NULL ) {
        SDL_SetError("sensor hasn't been opened yet");
        valid = 0;
    } else {
        valid = 1;
    }

    if ( sensor && sensor->closed )
    {
        valid = 0;
    }

    return valid;
}

/*
 * Get the number of multi-dimensional axis controls on a sensor
 */
int
SDL_SensorNumAxes(SDL_Sensor * sensor)
{
    if (!SDL_PrivatesensorValid(sensor)) {
        return (-1);
    }
    return (sensor->naxes);
}

/*
 * Get the current state of an axis control on a sensor
 */
Sint16
SDL_SensorGetAxis(SDL_Sensor * sensor, int axis)
{
    Sint16 state;

    if (!SDL_PrivatesensorValid(sensor)) {
        return (0);
    }
    if (axis < sensor->naxes) {
        state = sensor->axes[axis];
    } else {
        SDL_SetError("sensor only has %d axes", sensor->naxes);
        state = 0;
    }
    return (state);
}

/*
 * Get the current resolution of an axis control on a sensor
 */
Sint16
SDL_SensorGetResolution(SDL_Sensor * sensor, int axis)
{
    Sint16 state;

    if (!SDL_PrivatesensorValid(sensor)) {
        return (0);
    }
    if (axis < sensor->naxes) {
        state = sensor->resolution[axis];
    } else {
        SDL_SetError("sensor only has %d axes", sensor->naxes);
        state = 0;
    }
    return (state);
}

/*
 * Return if the sensor in question is currently attached to the system,
 *  \return 0 if not plugged in, 1 if still present.
 */
SDL_bool
SDL_SensorGetAttached(SDL_Sensor * sensor)
{
    if (!SDL_PrivatesensorValid(sensor)) {
        return SDL_FALSE;
    }

    return SDL_SYS_sensorAttached(sensor);
}

/*
 * Get the instance id for this opened sensor
 */
SDL_SensorID
SDL_SensorInstanceID(SDL_Sensor * sensor)
{
    if (!SDL_PrivatesensorValid(sensor)) {
        return (-1);
    }

    return (sensor->instance_id);
}

/*
 * Get the friendly name of this sensor
 */
const char *
SDL_SensorName(SDL_Sensor * sensor)
{
    if (!SDL_PrivatesensorValid(sensor)) {
        return (NULL);
    }

    return (sensor->name);
}

/*
 * Close a sensor previously opened with SDL_SensorOpen()
 */
void
SDL_SensorClose(SDL_Sensor * sensor)
{
    SDL_Sensor *sensorlist;
    SDL_Sensor *sensorlistprev;

    if (!sensor) {
        return;
    }

    /* First decrement ref count */
    if (--sensor->ref_count > 0) {
        return;
    }

    if (sensor == SDL_updating_sensor) {
        return;
    }

    SDL_SYS_sensorClose(sensor);

    sensorlist = SDL_Sensors;
    sensorlistprev = NULL;
    while ( sensorlist )
    {
        if (sensor == sensorlist)
        {
            if ( sensorlistprev )
            {
                // unlink this entry
                sensorlistprev->next = sensorlist->next;
            }
            else
            {
                SDL_Sensors = sensor->next;
            }

            break;
        }
        sensorlistprev = sensorlist;
        sensorlist = sensorlist->next;
    }

    if (sensor->name)
        SDL_free(sensor->name);

    /* Free the data associated with this sensor */
    if (sensor->axes) {
        SDL_free(sensor->axes);
    }
    if (sensor->resolutions) {
        SDL_free(sensor->resolutions);
    }
    SDL_free(sensor);
}

void
SDL_SensorQuit(void)
{
    /* Make sure we're not getting called in the middle of updating sensors */
    SDL_assert(!SDL_updating_sensor);

    /* Stop the event polling */
    while ( SDL_Sensors )
    {
        SDL_Sensors->ref_count = 1;
        SDL_SensorClose(SDL_Sensors);
    }

    /* Quit the sensor setup */
    SDL_SYS_sensorQuit();
}


/* These are global for SDL_syssensor.c and SDL_events.c */

int
SDL_PrivatesensorAxis(SDL_Sensor * sensor, Uint8 axis, Sint16 value)
{
    int posted;

    /* Make sure we're not getting garbage events */
    if (axis >= sensor->naxes) {
        return 0;
    }

    /* Update internal sensor state */
    if (value == sensor->axes[axis]) {
        return 0;
    }
    sensor->axes[axis] = value;

    /* Post the event, if desired */
    posted = 0;
#if !SDL_EVENTS_DISABLED
    if (SDL_GetEventState(SDL_JOYAXISMOTION) == SDL_ENABLE) {
        SDL_Event event;
        event.type = SDL_JOYAXISMOTION;
        event.jaxis.which = sensor->instance_id;
        event.jaxis.axis = axis;
        event.jaxis.value = value;
        posted = SDL_PushEvent(&event) == 1;
    }
#endif /* !SDL_EVENTS_DISABLED */
    return (posted);
}

int
SDL_PrivatesensorResolution(SDL_Sensor * sensor, Uint8 axis, Sint16 value)
{
    int posted;

    /* Make sure we're not getting garbage events */
    if (axis >= sensor->naxes) {
        return 0;
    }

    /* Update internal sensor state */
    if (value == sensor->resolutions[axis]) {
        return 0;
    }
    sensor->resolutions[axis] = value;

    /* Post the event, if desired */
    posted = 0;
#if !SDL_EVENTS_DISABLED
    if (SDL_GetEventState(SDL_JOYAXISMOTION) == SDL_ENABLE) {
        SDL_Event event;
        event.type = SDL_JOYAXISMOTION;
        event.jaxis.which = sensor->instance_id;
        event.jaxis.axis = axis;
        event.jaxis.value = value;
        posted = SDL_PushEvent(&event) == 1;
    }
#endif /* !SDL_EVENTS_DISABLED */
    return (posted);
}

void
SDL_SensorUpdate(void)
{
    SDL_Sensor *sensor;

    sensor = SDL_Sensors;
    while ( sensor )
    {
        SDL_Sensor *sensornext;
        /* save off the next pointer, the Update call may cause a sensor removed event
         * and cause our sensor pointer to be freed
         */
        sensornext = sensor->next;

        SDL_updating_sensor = sensor;

        SDL_SYS_sensorUpdate( sensor );

        if ( sensor->closed && sensor->uncentered )
        {
            int i;
            sensor->uncentered = 0;

            /* Tell the app that everything is centered/unpressed...  */
            for (i = 0; i < sensor->naxes; i++)
            {
                SDL_PrivatesensorAxis(sensor, i, 0);
                SDL_PrivatesensorResolution(sensor, i, 0);
            }
        }

        SDL_updating_sensor = NULL;

        /* If the sensor was closed while updating, free it here */
        if ( sensor->ref_count <= 0 ) {
            SDL_SensorClose(sensor);
        }

        sensor = sensornext;
    }

    SDL_SYS_sensorDetect();
}

/* return 1 if you want to run the sensor update loop this frame, used by hotplug support */
SDL_bool
SDL_PrivatesensorNeedsPolling()
{
    if (SDL_Sensors != NULL) {
        return SDL_TRUE;
    } else {
        return SDL_SYS_sensorNeedsPolling();
    }
}


/* return the guid for this index */
SDL_SensorGUID SDL_SensorGetDeviceGUID(int device_index)
{
    return SDL_SYS_sensorGetDeviceGUID( device_index );
}

/* return the guid for this opened device */
SDL_SensorGUID SDL_SensorGetGUID(SDL_Sensor * sensor)
{
    return SDL_SYS_sensorGetGUID( sensor );
}

/* convert the guid to a printable string */
void SDL_SensorGetGUIDString( SDL_SensorGUID guid, char *pszGUID, int cbGUID )
{
    static const char k_rgchHexToASCII[] = "0123456789abcdef";
    int i;

    if ((pszGUID == NULL) || (cbGUID <= 0)) {
        return;
    }

    for ( i = 0; i < sizeof(guid.data) && i < (cbGUID-1); i++ )
    {
        // each input byte writes 2 ascii chars, and might write a null byte.
        // If we don't have room for next input byte, stop
        unsigned char c = guid.data[i];

        *pszGUID++ = k_rgchHexToASCII[ c >> 4 ];
        *pszGUID++ = k_rgchHexToASCII[ c & 0x0F ];
    }
    *pszGUID = '\0';
}


/* vi: set ts=4 sw=4 expandtab: */