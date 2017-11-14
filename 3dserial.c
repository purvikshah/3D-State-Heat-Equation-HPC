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
  double diff;
  double epsilon = atof(argv[2]);
  int i;
  int iterations;
  int j,k;
  double mean;

  double *u[M][N];
  for (i=0; i<M; i++) 
    for(j=0; j<N; j++)
      u[i][j] = (double *)malloc(K * sizeof(double));

  double *w[M][N];
  for (i=0; i<M; i++)
    for(j=0; j<N; j++)
      w[i][j] = (double *)malloc(K * sizeof(double));


 // printf ( "  Spatial grid of %d by %d by %d points.\n", M, N, K );
 // printf ( "\n" );
 // printf ( "  The iteration will be repeated until the change is <= %f\n", epsilon );
  diff = epsilon;

/*
  Set the boundary values, which don't change.
*/
/* Front face*/
  ctime1 = omp_get_wtime();

  for ( i = 1; i < M - 1; i++ )
  {
    w[i][0][0] = 100.0;    // Or change the value here
  }
  for ( i = 1; i < M - 1; i++ )
  {
    w[i][N-1][0] = 100.0;
  }
  for ( j = 0; j < N; j++ )
  {
    w[M-1][j][0] = 100.0;
  }
  for ( j = 0; j < N; j++ )
  {
    w[0][j][0] = 0.0;
  }
//=============Front Face Done=========
  // Back Face

  for ( i = 1; i < M - 1; i++ )
  {
    w[i][0][K-1] = 100.0;    // Or change the value here
  }
  for ( i = 1; i < M - 1; i++ )
  {
    w[i][N-1][K-1] = 100.0;
  }
  for ( j = 0; j < N; j++ )
  {
    w[M-1][j][K-1] = 100.0;
  }
  for ( j = 0; j < N; j++ )
  {
    w[0][j][K-1] = 100.0;
  }
  //=========Back Face Done=========

 //left bottom
  for ( k = 0; k < K; k++ )
  {
    w[0][0][k] = 0.0;
  }
  // left top
  for ( k = 0; k < K; k++ )
  {
    w[0][N-1][k] = 100.0;
  }
  //right bottom
  for ( k = 0; k < K; k++ )
  {
    w[M-1][0][k] = 100.0;
  }
  // right top
  for ( k = 0; k < K; k++ )
  {
    w[M-1][N-1][k] = 100.0;
  }


/*
  Average the boundary values, to come up with a reasonable
  initial value for the interior.
*/
  mean = 0.0;
  
  // Front face
  for ( i = 1; i < M - 1; i++ )
  {
    mean = mean + w[i][0][0];
  }
  for ( i = 1; i < M - 1; i++ )
  {
    mean = mean + w[i][N-1][0];
  }
  for ( j = 0; j < N; j++ )
  {
    mean = mean + w[M-1][j][0];
  }
  for ( j = 0; j < N; j++ )
  {
    mean = mean + w[0][j][0];
  }
// =======Front face done=========
  // Back Face
  for ( i = 1; i < M - 1; i++ )
  {
    mean = mean + w[i][0][K-1];
  }
  for ( i = 1; i < M - 1; i++ )
  {
    mean = mean + w[i][N-1][K-1];
  }
  for ( j = 0; j < N; j++ )
  {
    mean = mean + w[M-1][j][K-1];
  }
  for ( j = 0; j < N; j++ )
  {
    mean = mean + w[0][j][K-1];
  }
//======Back Face DOne========
  //left bottom
  for ( k = 0; k < K; k++ )
  {
   mean = mean + w[0][0][k];
  }
  // left top
  for ( k = 0; k < K; k++ )
  {
    mean = mean + w[0][N-1][k];
  }
  //right bottom
  for ( k = 0; k < K; k++ )
  {
    mean = mean + w[M-1][0][k];
  }
  // right top
  for ( k = 0; k < K; k++ )
  {
    mean = mean + w[M-1][N-1][k];
  }

  mean = mean / ( double ) ( 4 * M + 4 * N + 4 * K - 12 );
/*
  Initialize the interior solution to the mean value.
*/
  for ( i = 1; i < M - 1; i++ )
  {
    for ( j = 1; j < N - 1; j++ )
        for ( k =1; k < K - 1; k++)
        {
          w[i][j][k] = mean;
        }
  }
/*
  iterate until the  new solution W differs from the old solution U
  by no more than EPSILON.
*/
  iterations = 0;

  while ( epsilon <= diff )
  {

  //Save the old solution in U.
  	
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
 //See the difference is less than epsilon or not
  	diff = 0.0;
  	for ( i = 1; i < M - 1; i++ )
  	{
  		for ( j = 1; j < N - 1; j++ )
  		{
  			for ( k = 1; k < K - 1; k++)
  			{

          w[i][j][k] = ( u[i-1][j][k] + u[i+1][j][k] + u[i][j-1][k] + u[i][j+1][k] + u[i][j][k+1] + u[i][j][k-1] ) / 6.0;
  				if ( diff < fabs ( w[i][j][k] - u[i][j][k] ) )
  				{
  					diff = fabs ( w[i][j][k] - u[i][j][k] );
  				}
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


