# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# include <omp.h>
#include <time.h>


int main ( int argc, char *argv[] )
{
  int M = atoi(argv[1]);
  int N = atoi(argv[1]);

  double diff;
  double epsilon = atof(argv[2]);
  int i;
  int iterations;
  int j;
  double mean;
  double my_diff;
  double **u  = (double **)malloc(M * sizeof(double *));
  for (i=0; i<M; i++)
     u[i] = (double *)malloc(N * sizeof(double));

  double **w  = (double **)malloc(M * sizeof(double *));
  for (i=0; i<M; i++)
     w[i] = (double *)malloc(N * sizeof(double));
  double wtime;

  
  //printf ( "  Spatial grid of %d by %d points.\n", M, N );
  //printf ( "  The iteration will be repeated until the change is <= %e\n", epsilon ); 
/*
  Set the boundary values, which don't change. 
*/
  mean = 0.0;
  double t1 = omp_get_wtime();
  {

    for ( i = 1; i < M - 1; i++ )
    {
      w[i][0] = 100.0;
    }

    for ( i = 1; i < M - 1; i++ )
    {
      w[i][N-1] = 100.0;
    }

    for ( j = 0; j < N; j++ )
    {
      w[M-1][j] = 100.0;
    }

    for ( j = 0; j < N; j++ )
    {
      w[0][j] = 0.0;
    }
/*
  Average the boundary values, to come up with a reasonable
  initial value for the interior.
*/

    for ( i = 1; i < M - 1; i++ )
    {
      mean = mean + w[i][0] + w[i][N-1];
    }

    for ( j = 0; j < N; j++ )
    {
      mean = mean + w[M-1][j] + w[0][j];
    }
  }
  mean = mean / ( double ) ( 2 * M + 2 * N - 4 );
  //printf ( "\n" );
  //printf ( "  MEAN = %f\n", mean );
/* 
  Initialize the interior solution to the mean value.
*/

  {

    for ( i = 1; i < M - 1; i++ )
    {
      for ( j = 1; j < N - 1; j++ )
      {
        w[i][j] = mean;
      }
    }
  }
/*
  iterate until the  new solution W differs from the old solution U
  by no more than EPSILON.
*/
  iterations = 0;

  diff = epsilon;

  while ( epsilon <= diff )
  {

    {
/*
  Save the old solution in U.
*/

      for ( i = 0; i < M; i++ ) 
      {
        for ( j = 0; j < N; j++ )
        {
          u[i][j] = w[i][j];
        }
      }
/*
  Determine the new estimate of the solution at the interior points.
  The new solution W is the average of north, south, east and west neighbors.
*/

      for ( i = 1; i < M - 1; i++ )
      {
        for ( j = 1; j < N - 1; j++ )
        {
          w[i][j] = ( u[i-1][j] + u[i+1][j] + u[i][j-1] + u[i][j+1] ) / 4.0;
        }
      }
    }
    diff = 0.0;

    {
      my_diff = 0.0;

      for ( i = 1; i < M - 1; i++ )
      {
        for ( j = 1; j < N - 1; j++ )
        {
          if ( my_diff < fabs ( w[i][j] - u[i][j] ) )
          {
            my_diff = fabs ( w[i][j] - u[i][j] );
          }
        }
      }

      {
        if ( diff < my_diff )
        {
          diff = my_diff;
        }
      }
    }

    iterations++;
  } 
  wtime = omp_get_wtime ( ) - t1;

  printf ( "%d %f\n",iterations,wtime );
  return 0;
}

