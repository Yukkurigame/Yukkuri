#ifndef YDEFINE_H
#define YDEFINE_H

//TODO: All to lua

#define WWIDTH	800
#define	WHEIGHT 600

#define MAXIMUM_FRAME_RATE 90
#define MINIMUM_FRAME_RATE 30
#define UPDATE_INTERVAL (1.0 / MAXIMUM_FRAME_RATE)
#define MAX_CYCLES_PER_FRAME (MAXIMUM_FRAME_RATE / MINIMUM_FRAME_RATE)

#define IMAGEPATH "data/images/"
#define DEFAULT_IMAGE "white.png"

#define DAY_LENGTH 20

#define ACTION_INTERVAL 3000

//#define JOYSTICKENABLE
//#define JSSENS 200

#endif //YDEFINE_H
