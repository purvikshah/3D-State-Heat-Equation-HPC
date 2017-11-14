# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# include <omp.h>
#include <time.h>


int main ( int argc, char *argv[] )

{
  int M = atoi(argv[1]);
  int N = atoi(argv[1]);
  int K = atoi(argv[1]);

  double ctime;
  double ctime1;
  double ctime2;
  double diff,th_diff;
  double epsilon = atof(argv[2]);
  int num_threads = atoi(argv[3]);
  int i;
  int iterations;
  int j,k;
  double mean;
  omp_set_num_threads(num_threads);

  double *u[M][N];
  for (i=0; i<M; i++) 
    for(j=0; j<N; j++)
      u[i][j] = (double *)malloc(K * sizeof(double));

  double *w[M][N];
  for (i=0; i<M; i++)
    for(j=0; j<N; j++)
      w[i][j] = (double *)malloc(K * sizeof(double));
  

  //printf ( " Spatial grid of %d by %d by %d points.\n", M, N, K );
  //printf ( "\n" );
  //printf ( " The iteration will be repeated until the change is <= %f\n", epsilon );
  diff = epsilon;

/*
  Set the boundary values, which don't change.
*/
/* Front face*/
 mean = 0.0;
 ctime1 = omp_get_wtime();
#pragma omp parallel shared ( w ) private ( i, j, k )
{
 #pragma omp for
  for ( i = 1; i < M - 1; i++ )
  {
    w[i][0][0] = 100.0;    // Or change the value here
  }
  #pragma omp for
  for ( i = 1; i < M - 1; i++ )
  {
    w[i][N-1][0] = 100.0;
  }
  #pragma omp for
  for ( j = 0; j < N; j++ )
  {
    w[M-1][j][0] = 100.0;
  }
  #pragma omp for
  for ( j = 0; j < N; j++ )
  {
    w[0][j][0] = 0.0;
  }
  //=============Front Face Done=========
  // Back Face
  #pragma omp for
  for ( i = 1; i < M - 1; i++ )
  {
    w[i][0][K-1] = 100.0;    // Or change the value here
  }
  #pragma omp for
  for ( i = 1; i < M - 1; i++ )
  {
    w[i][N-1][K-1] = 100.0;
  }
  #pragma omp for
  for ( j = 0; j < N; j++ )
  {
    w[M-1][j][K-1] = 100.0;
  }
  #pragma omp for
  for ( j = 0; j < N; j++ )
  {
    w[0][j][K-1] = 100.0;
  }

  //=========Back Face Done=========

 //left bottom
  #pragma omp for
  for ( k = 0; k < K; k++ )
  {
    w[0][0][k] = 0.0;
  }
  #pragma omp for
  // left top
  for ( k = 0; k < K; k++ )
  {
    w[0][N-1][k] = 100.0;
  }
  #pragma omp for
  //right bottom
  for ( k = 0; k < K; k++ )
  {
    w[M-1][0][k] = 100.0;
  }
  #pragma omp for
  // right top
  for ( k = 0; k < K; k++ )
  {
    w[M-1][N-1][k] = 100.0;
  }


/*
  Average the boundary values, to come up with a reasonable
  initial value for the interior.
*/
 
  // Front face
  #pragma omp for reduction ( + : mean )
  for ( i = 1; i < M - 1; i++ )
  {
    mean = mean + w[i][0][0];
  }
  #pragma omp for reduction ( + : mean )
  for ( i = 1; i < M - 1; i++ )
  {
    mean = mean + w[i][N-1][0];
  }
  #pragma omp for reduction ( + : mean )
  for ( j = 0; j < N; j++ )
  {
    mean = mean + w[M-1][j][0];
  }
  #pragma omp for reduction ( + : mean )
  for ( j = 0; j < N; j++ )
  {
    mean = mean + w[0][j][0];
  }
// =======Front face done=========
  // Back Face
  #pragma omp for reduction ( + : mean )
  for ( i = 1; i < M - 1; i++ )
  {
    mean = mean + w[i][0][K-1];
  }
  #pragma omp for reduction ( + : mean )
  for ( i = 1; i < M - 1; i++ )
  {
    mean = mean + w[i][N-1][K-1];
  }
  #pragma omp for reduction ( + : mean )
  for ( j = 0; j < N; j++ )
  {
    mean = mean + w[M-1][j][K-1];
  }
  #pragma omp for reduction ( + : mean )
  for ( j = 0; j < N; j++ )
  {
    mean = mean + w[0][j][K-1];
  }

//======Back Face DOne========
  //left bottom
  #pragma omp for reduction ( + : mean )
  for ( k = 0; k < K; k++ )
  {
   mean = mean + w[0][0][k];
  }
  #pragma omp for reduction ( + : mean )
  // left top
  for ( k = 0; k < K; k++ )
  {
    mean = mean + w[0][N-1][k];
  }
  #pragma omp for reduction ( + : mean )
  //right bottom
  for ( k = 0; k < K; k++ )
  {
    mean = mean + w[M-1][0][k];
  }
  #pragma omp for reduction ( + : mean )
  // right top
  for ( k = 0; k < K; k++ )
  {
    mean = mean + w[M-1][N-1][k];
  }
}
  mean = mean / ( double ) ( 4 * M + 4 * N + 4 * K  - 12 );
/*
  Initialize the interior solution to the mean value.
*/
#pragma omp parallel shared ( mean, w ) private ( i, j, k )
  {
    #pragma omp for
    for ( i = 1; i < M - 1; i++ )
    {
      for ( j = 1; j < N - 1; j++ )
      {
        for ( k =1; k < K - 1; k++)
        {
          w[i][j][k] = mean;
        }
      }
    }
  }
/*
  iterate until the  new solution W differs from the old solution U
  by no more than EPSILON.
*/
  iterations = 0;
  

  while ( epsilon <= diff )
  {
/*
  Save the old solution in U.
*/
# pragma omp parallel shared ( diff, u, w ) private ( i, j,k, th_diff )
    {
      # pragma omp for
      for ( i = 0; i < M; i++ )
      {
        for ( j = 0; j < N; j++ )
        {
          for( k = 0 ; k < K ; k++)
          {
            u[i][j][k] = w[i][j][k];
          }
        }
      }

   //  Determine the new estimate of the solution at the interior points.

//  The new solution W is the average of north, south, east and west, upper, lower neighbors.
        # pragma omp for
      for ( i = 1; i < M - 1; i++ )
      {
        for ( j = 1; j < N - 1; j++ )
        {
          for ( k = 1; k < K - 1; k++)
          {
            w[i][j][k] = ( u[i-1][j][k] + u[i+1][j][k] + u[i][j-1][k] + u[i][j+1][k] + u[i][j][k+1] + u[i][j][k-1] ) / 6.0;
          }
        }
      }
    }
    diff = 0.0;
 //See the difference is less than epsilon or not
# pragma omp parallel shared ( diff, u, w ) private ( i, j, k,th_diff )
    {

      th_diff = 0.0;
       # pragma omp for
      for ( i = 1; i < M - 1; i++ )
      {
        for ( j = 1; j < N - 1; j++ )
        {
          for ( k = 1; k < K - 1; k++)
          {
            if ( th_diff < fabs ( w[i][j][k] - u[i][j][k] ) )
            {
              th_diff = fabs ( w[i][j][k] - u[i][j][k] );
            }
          }
        }
      }
        # pragma omp critical
      {
        if ( diff < th_diff )
        {
          diff = th_diff;
        }
      }
    }
    iterations++;
  }
  ctime2 = omp_get_wtime();
  ctime = ctime2 - ctime1;

  printf ( "%d %f\n", iterations, ctime );
  return 0;
}
