
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "auto_management.h"
#include "dcn.h"
#include "messages.h"
#include "utils/utils.h"
#include "purplemow.h"
#include "mow.h"
#include "sensors.h"
#include "modules.h"
#include "config.h"

#include "test.h"
#include "test_thread.h"

#define DO_ARGS     0
#define DO_I2C      1
#define DO_COMM     1
#define DO_DCN      1
#define DO_NET      0
#define DO_SENSORS  1
#define DO_CONFIG   1

#define DO_TEST          0
#define DO_TEST_THREADS  0

// Set this to 1 to enable debug of main()
#define DEBUG_MAIN  0

/**
 * @defgroup purplemow PurpleMow Main
 * PurpleMow main function.
 */

/**
 * purplemow
 *
 * @ingroup purplemow
 */
struct purplemow {
    int                     debug;
};

static struct purplemow this = { .debug = DEBUG_MAIN };

/**
 * Main function, initializes and starts all the modules.
 *
 * @ingroup purplemow
 *
 * @param[in] argc      Agrument count
 * @param[in] argv      Argument vector
 *
 * @return              Success status
 */
int main(int argc, char **argv)
{
    int state;

#if DO_ARGS
    // args on the command line
    if ( argc < 2 )
    {
        printf("Wrong\n");
        exit(0);
    }
#endif // DO_ARGS

    /*************
     *  I N I T  *
     *************/

    if ( this.debug )
        printf("Initializing... ");

    modules_init();

    message_init();
    cli_init();

#if DO_CONFIG
    config_init();
#endif // DO_CONFIG

#if DO_I2C
    // i2c stuff
    io_init();
#endif // DO_I2C

#if DO_COMM
    communicator_init();
#endif // DO_COMM

#if DO_SENSORS
    sensors_init();
#endif // DO_SENSORS

#if DO_DCN
    dcn_init();
#endif // DO_DCN

#if DO_NET
    multicast_init();
#endif // DO_NET

#if DO_TEST
    test_init();
#endif // DO_TEST

#if DO_TEST_THREADS
    test_thread_init();
#endif // DO_TEST_THREADS

    mow_init();

    if ( this.debug )
        printf("OK\n");

    /**************
     *  S T A R T *
     **************/

    if ( this.debug )
        printf("Registering commands... ");

    modules_run_phase(phase_REGISTER_COMMANDS);

    if ( this.debug )
        printf("OK\n");

    if ( this.debug )
        printf("Loading configuration... ");

    modules_run_phase(phase_REGISTER_VALUES);
    modules_run_phase(phase_LOAD_DEFAULT_VAULES);
    modules_run_phase(phase_LOAD_CONFIG);

    if ( this.debug )
        printf("OK\n");

    if ( this.debug )
        printf("Starting... ");

    modules_run_phase(phase_START);

    if ( this.debug )
        printf("OK\n");

    if ( this.debug )
        printf("Starting sensors... ");

    modules_run_phase(phase_START_SENSORS);

    if ( this.debug )
        printf("OK\n");

    modules_run_phase(phase_MOW);

    while ( 1 )
        sleep(10);
}

