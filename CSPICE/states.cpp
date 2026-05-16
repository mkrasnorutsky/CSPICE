#ifdef INCLUDE_GARBAGE_AND_SO
#include <stdio.h>
#include "SpiceUsr.h"
#include <CSPICE/Asteroids.h>

int main()
{
   #define     FILE_SIZE 128
   #define     WORD_SIZE 80


   /*
   Local variables.
   */

   SpiceDouble    state[6];
   SpiceDouble    etbeg;


   SpiceChar      targ  [WORD_SIZE];
   SpiceChar      utc   [WORD_SIZE];
   SpiceChar      frame [WORD_SIZE];

   SpiceChar      format[] = "c";

   SpiceInt       maxpts   = 0;
   SpiceInt       prec     = 0;
   SpiceInt       i;
	
   furnsh_c ("/naif0009.tls");
   //furnsh_c ("/file.bsp");
   prompt_c ( "Enter the name of a target body: ",  WORD_SIZE, targ  );

	puts (" ");
      /*
      Compute the state of 'targ' from 'obs' at 'et' in the 'frame'
      reference frame and aberration correction 'abcorr'.
      */
      //spkezr_c ( targ, etbeg, "J2000", "NONE", "sun", state, &lt );
	//etbeg = 2447893;
	etbeg = 2451545;
	//planet_pv(targ, etbeg, state);
	char error[1] = {0};
	AsteroidResult result = asteroids_calculate(etbeg, 299, 0, error, sizeof(error));
	if (result.valid)
	{
		state[0] = result.pos_x;
		state[1] = result.pos_y;
		state[2] = result.pos_z;
		state[3] = result.vel_x;
		state[4] = result.vel_y;
		state[5] = result.vel_z;
	}

      /*
      Convert the ET (ephemeris time) into a UTC time string
      for displaying on the screen.
      */
      et2utc_c ( (etbeg - 2451545.5) * spd_c(), format, prec, WORD_SIZE, utc );

      /* 
      Display the results of the state calculation.
      */
      printf ( "For time %d of %d, the state of:\n", i, maxpts );

      printf ( "Body            : %s\n", targ );

      printf ( "In Frame        : %s\n", frame );

      printf ( "At UTC time     : %s\n", utc );

      puts  (" ");
      printf( "                 Position (km)              ");
      printf( "Velocity (km/s)\n"                           );
      printf( "            -----------------------     "    );
      printf( "-----------------------\n" );

      printf( "          X: %23.16e     %26.16e\n", state[0],
                                                    state[3] );
      printf( "          Y: %23.16e     %26.16e\n", state[1],
                                                    state[4] );
      printf( "          Z: %23.16e     %26.16e\n", state[2],
                                                    state[5] );
      printf( "  MAGNITUDE: %23.16e     %23.16e\n", 
                                                vnorm_c(state),
                                                vnorm_c(state+3) );

   return 0;
}
#endif
