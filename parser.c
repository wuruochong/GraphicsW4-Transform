#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"


/*======== void parse_file () ==========
Inputs:   char * filename
          struct matrix * transform,
          struct matrix * pm,
          screen s
Returns:

Goes through the file named filename and performs all of the actions listed in that file.
The file follows the following format:
     Every command is a single character that takes up a line
     Any command that requires arguments must have those arguments in the second line.
     The commands are as follows:
         line: add a line to the edge matrix -
	    takes 6 arguemnts (x0, y0, z0, x1, y1, z1)
	 ident: set the transform matrix to the identity matrix -
	 scale: create a scale matrix,
	    then multiply the transform matrix by the scale matrix -
	    takes 3 arguments (sx, sy, sz)
	 translate: create a translation matrix,
	    then multiply the transform matrix by the translation matrix -
	    takes 3 arguments (tx, ty, tz)
	 rotate: create an rotation matrix,
	    then multiply the transform matrix by the rotation matrix -
	    takes 2 arguments (axis, theta) axis should be x y or z
	 apply: apply the current transformation matrix to the
	    edge matrix
	 display: draw the lines of the edge matrix to the screen
	    display the screen
	 save: draw the lines of the edge matrix to the screen
	    save the screen to a file -
	    takes 1 argument (file name)
	 quit: end parsing

See the file script for an example of the file format


IMPORTANT MATH NOTE:
the trig functions int math.h use radian mesure, but us normal
humans use degrees, so the file will contain degrees for rotations,
be sure to conver those degrees to radians (M_PI is the constant
for PI)
====================*/
void parse_file ( char * filename,
                  struct matrix * transform,
                  struct matrix * edges,
                  screen s) {

  FILE *f;
  char line[256];
  clear_screen(s);
  color c;

  c.red = 0;
  c.green = MAX_COLOR;
  c.blue = 0;
  // printf("parse_file running\n");

  int mode = 0;  //0 = normal, 1 = reading line args, 2 = reading scale args, 3 = reading rot args, 4 = reading transformation args

  if ( strcmp(filename, "stdin") == 0 )
    f = stdin;
  else
    f = fopen(filename, "r");

  while ( fgets(line, 255, f) != NULL ) {
    line[strlen(line)-1]='\0';
    printf(":%s:\n",line);
    if (!mode){

      if(strcmp("ident",line)==0){
        ident(transform);
        printf("after ident\n");
        print_matrix(transform);
      }

      else if(strcmp("apply",line)==0){
        printf("transform: \n");
        print_matrix(transform);
        printf("edges: \n");
        print_matrix(edges);
        matrix_mult(transform, edges);
        printf("result \n");
        print_matrix(edges);
      }

      else if(strcmp("display",line)==0){
        // print_matrix(edges);
        clear_screen(s);
        draw_lines(edges,s,c);
        display(s);
        save_extension(s,"lines.png");
      }

      else if(strcmp("quit",line)==0){
        return;   //Is this right?
      }

      else if(strcmp("line",line)==0){
        mode = 1;
      }

      else if(strcmp("scale",line)==0){
        mode = 2;
      }

      else if(strcmp("rotate",line)==0){
        mode = 3;
      }

      else if(strcmp("translate",line)==0){
        mode = 4;
      }
    }

    else if(mode == 1){
      char * tmpline = strdup(line);
      double x0;
      double y0;
      double z0;
      double x1;
      double y1;
      double z1;
      x0 = atof(strsep(&tmpline," "));
      y0 = atof(strsep(&tmpline," "));
      z0 = atof(strsep(&tmpline," "));
      x1 = atof(strsep(&tmpline," "));
      y1 = atof(strsep(&tmpline," "));
      z1 = atof(tmpline);
      // printf("%d %d %d %d %d %d\n",x0,y0,z0,x1,y1,z1);
      add_edge(edges,x0,y0,z0,x1,y1,z1);
      mode=0;
    }

    else if(mode == 2){
      char * tmpline = strdup(line);
      double sx;
      double sy;
      double sz;
      sx = atof(strsep(&tmpline," "));
      sy = atof(strsep(&tmpline," "));
      sz = atof(tmpline);
      // printf("numbers: %d %d %d",sx,sy,sz);
      struct matrix *tmpmatrix = make_scale(sx,sy,sz);
      matrix_mult(tmpmatrix,transform);
      mode = 0;
    }

    else if(mode == 3){
      char * tmpline = strdup(line);
      char * ax;
      double t;
      struct matrix *tmpmatrix;
      ax = strsep(&tmpline, " ");
      t = atof(tmpline);
      if (strcmp(ax,"x")==0){
        tmpmatrix = make_rotX(t);
        matrix_mult(tmpmatrix,transform);
      }
      else if (strcmp(ax,"y")==0){
        tmpmatrix = make_rotY(t);
        matrix_mult(tmpmatrix,transform);
      }
      else if (strcmp(ax,"z")==0){
        tmpmatrix = make_rotZ(t);
        matrix_mult(tmpmatrix,transform);
      }
      mode = 0;
    }

    else if(mode == 4){
      char * tmpline = strdup(line);
      double tx;
      double ty;
      double tz;
      tx = atof(strsep(&tmpline," "));
      ty = atof(strsep(&tmpline," "));
      tz = atof(tmpline);
      struct matrix *tmpmatrix = make_translate(tx,ty,tz);
      matrix_mult(tmpmatrix,transform);
      mode = 0;
    }
  }
}
