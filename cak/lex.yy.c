# include "stdio.h"
# define U(x) x
# define NLSTATE yyprevious=YYNEWLINE
# define BEGIN yybgin = yysvec + 1 +
# define INITIAL 0
# define YYLERR yysvec
# define YYSTATE (yyestate-yysvec-1)
# define YYOPTIM 1
# define YYLMAX BUFSIZ
# define output(c) putc(c,yyout)
# define input() (((yytchar=yysptr>yysbuf?U(*--yysptr):getc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar)
# define unput(c) {yytchar= (c);if(yytchar=='\n')yylineno--;*yysptr++=yytchar;}
# define yymore() (yymorfg=1)
# define ECHO fprintf(yyout, "%s",yytext)
# define REJECT { nstr = yyreject(); goto yyfussy;}
int yyleng; extern char yytext[];
int yymorfg;
extern char *yysptr, yysbuf[];
int yytchar;
FILE *yyin = {stdin}, *yyout = {stdout};
extern int yylineno;
struct yysvf { 
	struct yywork *yystoff;
	struct yysvf *yyother;
	int *yystops;};
struct yysvf *yyestate;
extern struct yysvf yysvec[], *yybgin;
 ;;; -*- Mode: LISP;  Package: hacks; base: 8; lowercase: t -*-
  (multiple-value-bind (nil rem) (floor n d) rem))
  `(as-2-reverse (1+ (ar-2-reverse *color-screen-array* ,x1 ,y1))
		 *color-screen-array*
		 ,x1
		 ,y1))
			 &aux (max (max (abs (- xn x0)) (abs (- yn y0)))))
       (draw-sym-subline x0 y0 xn yn 0 max))
				    &aux (max (max (abs (- xn x0)) (abs (- yn y0)))))
       (draw-sym-subline x0 y0 xn yn
			 (- (fix (* -1 begfrac max)))
			 (fix (* endfrac max))))
       (cond ((> xn x0) (cond ((> yn y0) (cond ((> dx dy) (line-loop #'plot0 x0 y0 dx dy i j))
					       ((line-loop #'plot1 y0 x0 dy dx i j))))
			      ((cond ((> dx dy) (line-loop #'plot7 x0 (- y0) dx dy i j))
				     ((line-loop #'plot6 (- y0) x0 dy dx i j))))))
	     ((cond ((> yn y0) (cond ((> dx dy) (line-loop #'plot3 (- x0) y0 dx dy i j))
				     ((line-loop #'plot2 y0 (- x0) dy dx i j))))
		    ((cond ((> dx dy) (line-loop #'plot4 (- x0) (- y0) dx dy i j))
			   ((line-loop #'plot5 (- y0) (- x0) dy dx i j))))))))
		      &aux (num (+ dx (* 2 i dy))))
       (do ((j2 (min j (ash dx -1)))
	    (y (+ y0 (truncate num (ash dx 1))))
	    (i i (1+ i))
	    (x (+ x0 i) (1+ x))
	    (f (ash (- (\ num (ash dx 1)) dx) -1) (+ f dy)))
	   ((> i j2) (do ((i i (1+ i))
			  (x x (1+ x))
			  (f f (+ f dy)))
			 ((> i j))
			 (and (> (+ f f) dx) (setq f (- f dx) y (1+ y)))
			 (funcall fun x y)))
	   (and ( (+ f f) dx) (setq f (- f dx) y (1+ y)))
	   (funcall fun x y)))
			    &optional (dx (abs (- xn x0))) (dy (abs (- yn y0))))
       (cond ((> xn x0) (cond ((> yn y0) (cond ((> dx dy)
						(line-clip #'plot0 x0 y0 dx dy xe ye xf yf))
					       ((line-clip #'plot1 y0 x0 dy dx ye xe yf xf))))
			      ((cond ((> dx dy)
				      (line-clip #'plot7 x0 (- y0) dx dy xe (- yf) xf (- ye)))
				     ((line-clip #'plot6 (- y0) x0 dy dx (- yf) xe (- ye) xf))))))
	     ((cond ((> yn y0)
		     (cond ((> dx dy)
			    (line-clip #'plot3 (- x0) y0 dx dy (- xf) ye (- xe) yf))
			   ((line-clip #'plot2 y0 (- x0) dy dx ye (- xf) yf (- xe)))))
		    ((cond ((> dx dy)
			    (line-clip #'plot4 (- x0) (- y0) dx dy (- xf) (- yf) (- xe) (- ye)))
			   ((line-clip #'plot5 (- y0) (- x0) dy dx (- yf) (- xf) (- ye) (- xe)))))))))
		      &aux (x (max x0 xe (if (= dy 0) xe (+ x0 (//+ (* dx
								       (1- (ash (- ye y0) 1)))
								    (ash dy 1))))))
		           (num (+ dx (* 2 dy (- x x0))))
			   (lx (min xf (if (= dy 0) xf (+ x0 (//+ (* dx (1- (ash (- yf y0) 1)))
								 (ash dy 1)))))))
       (do ((xx (min (+ x0 (ash dx -1)) lx))
	    (y (+ y0 (//- num (ash dx 1))))
	    (x x (1+ x))
	    (f (ash (- (\- num (ash dx 1)) dx) -1) (+ f dy)))
	   ((> x xx) (do ((xx lx)
			  (x x (1+ x))
			  (f f (+ f dy)))
			 ((> x xx))
			 (and (> (+ f f) dx) (setq f (- f dx) y (1+ y)))
			 (funcall fun x y)))
	   (and ( (+ f f) dx) (setq f (- f dx) y (1+ y)))
	   (funcall fun x y)))
