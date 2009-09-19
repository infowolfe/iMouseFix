/* -----------------------------------------------------------------
 * MouseFix
 *
 * Version 1.2.1 - 19/09/2009
 *
 * Update for 10.6 IOKit, since NX* seem to be missing now.
 *
 * A program to disable/adjust the annoying mouse acceleration on Mac OS-X
 *
 * This routine will allow setting of the mouse scaling and acceleration
 * to something usable, such as a 'normal' linear releationship rather than
 * the hit-and-miss and otherwise unadjustable (and incredibly annoying!)
 * setting that is provided as standard
 *
 * HOW TO USE
 * ----------
 * Simple! Just run it. The defaults defined work ok for me on a 20" G5 iMac
 * The default acceleration factor may be overridden by supplying it as a
 * parameter on the command line.
 *
 * However, if you want to use or adjust the scaling table then you will have
 * to change the values in the code and recompile.
 *
 * Copyright (c) 2005 - Richard Bentley
 *
 * TERMS OF USE
 * ------------
 * This software is provided as-is and is used entirely at your own risk.
 *
 * This software may be freely distributed, however no payment may be demanded
 * or offered for it (either monetary, in kind, through offers of any weird
 * sex acts, or anything else).
 *
 * -----------------------------------------------------------------
 */

/* ----------------------------------------------------------------- */
#include <stdlib.h>
#include <IOKit/hidsystem/event_status_driver.h>
#include <stdio.h>

/* ----------------------------------------------------------------- */
/* ### Modify the following values to suite your preferences ### */

/* Mouse acceleration factor
 * I'm not sure how this is actually interpreted, but I assume it specifies
 * how keen the mouse driver is to traverse its way up the scaling table
 * I don't know if it depends on how many entries there are in the scaling
 * table
 *
 * NOTE: Although this is called an 'acceleration' factor, if the scaling
 * ----  table is disabled, it actually seems to act as a simple scaling factor
 *       resulting in a simple, scaled linear mouse / cursor relationship
 *
 * This value should always be positive
 */
static float accel_factor = 2.0;

/* Setting this value to '1' will enable use of the scaling table
 * Setting this value to '0' will disable use of the scaling table and
 * the values set for it shall be ignored
 */
#define USE_SCALING_TABLE (1)

/* Scaling table. This controls the scaling relationship between mouse
 * movement and screen cursor movement
 *
 * This consists of threshold / scale pairs of values. These are interpreted
 * as follows:
 *
 *  eg: {2, 3} ...means...
 *
 *      If the mouse movess 2 units then  the screen cursor is moved 2 x 3 = 6
 *      pixels on screen.
 *
 * If the table includes more than one entry pair then the threshold (first value
 * of the pair) must be larger than the previous threshold value in the table
 *
 * If the first entry in this table does not have a threshold of 1 then  I
 * assume the driver assumes (generates internally) a scale of {1, 1}, but
 * I'm not sure (I got fed up with trying to make sense of the driver code to
 * be certain)
 *
 * Note that these value are signed to match the sloppy mouse driver API, but
 * all values should be positive!
 *
 * A maximum of NX_MAXMOUSESCALINGS entry pairs may be specified
 * At the time of writing this is 20 and is defined in...
 *
 * /System/Library/Frameworks/IOKit.framework/Versions//A/Headers/hidsystem/IOHIDTypes.h
 */
#if USE_SCALING_TABLE

/* The values used here have been suggested by Aaron Johnson, based on the MS Windows XP
 * mousecurve data. To quote from Aaron Johnson :-
 *
 * "I've been using Mousefix for some time now and I must say it does a very good job at
 *  taming the MacOSX mouse. I recently stumbled over a Microsoft usability study on mice,
 *  this document also shows the Windows XP mousecurve. After dissecting the tables and
 *  diagrams I have come up with the following scaling table for MouseFix.
 *  It seems to work best using an accel_factor of 2"
 */
static const short scale[][2] =
{
    {1,      1},
    {40.18,  6.59},
    {123.48, 4.04},
    {377.30, 6.31},
    {3920,   14.15}
};

#endif

/* -----------------------------------------------------------------
 * MouseFix
 *
 * Parameters:
 * argc - See K&R :-)
 * argv
 *
 * Returns:
 * 0 - OK - Mouse movement adjusted
 * 1 - Failed
 */
int main(int argc, const char** argv)
{
    int fail = 0;

    /* Acceleration factor may be specified on the command line. If it is, extract it */
    if (argc > 1)
    {
        float par_accel_factor = atof(argv[1]);
        if (par_accel_factor <= 0.0)
        {
            /* Illegal acceleration factor supplied - help the user out */
            printf("\nMouseFix: Fix Mac OS-X's mouse movement\n"
                   "Synopsis: %s {accel}\n\n", argv[0]);
            printf("Where:\n"
                   "accel - This is optional and if specified must be a positive\n"
                   "        floating point mouse acceleration factor. The default\n"
                   "        value is %0.2f\n\n", accel_factor);
            printf("        NOTE: Although this is called an 'acceleration' factor,\n"
                   "        ----  if the scaling table is disabled (which it is by\n"
                   "              default unless you have modified the code, it\n"
                   "              actually seems to act as a simple scaling factor\n"
                   "              resulting in a simple, scaled linear mouse / cursor\n"
                   "              relationship (which, if you are using this, is\n"
                   "              probably what you want)\n\n"
                   "By default, the mouse scaling table is enabled, resulting in\n"
                   "a non-linear mouse/cursor relationship. To stop using the mouse scaling\n"
                   "table, you will need to adjust the configuration in the code and\n"
                   "recompile\n\n");
            fail = 1;
        }

        accel_factor = par_accel_factor;
    }

    if (!fail)
    {
        /* Mouse device handle */
        NXEventHandle evs;

        evs = NXOpenEventStatus();

        if(!evs)
        {
            printf("MouseFix: Could not establish connection to mouse device\n");
            fail = 1;
        }
        else
        {
            NXMouseScaling ms;
            unsigned int x;

            /* Set the acceleration factor */
            IOHIDSetMouseAcceleration((io_connect_t)evs, accel_factor);

#if USE_SCALING_TABLE
            /* Create the mouse scaling table */
            ms.numScaleLevels = sizeof(scale) >> 2;

            if (ms.numScaleLevels <= NX_MAXMOUSESCALINGS)
            {
                for (x = 0; x < ms.numScaleLevels; ++x)
                {
                    ms.scaleThresholds[x] = scale[x][0];
                    ms.scaleFactors[x] = scale[x][1];
                }

				IOHIDSetMouseAcceleration(evs, &ms);
            }
            else
            {
                printf("MouseFix: Too many scaling factor pairs defined.\n"
                       "          Maximum allowed = %d\n", NX_MAXMOUSESCALINGS);
                fail = 1;
            }
#else
            /* Disable the mouse scaling table */
            ms.numScaleLevels = 0;
			IOHIDSetMouseAcceleration(evs, &ms);
#endif

            /* Tidy up */
            NXCloseEventStatus(evs);
        }
    }

    return fail;
}

// eof
