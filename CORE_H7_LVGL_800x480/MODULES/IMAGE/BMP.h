#ifndef __IMAGE_H
#define __IMAGE_H

typedef struct
{
  uint16_t bfType;  		//Отметка для отличия формата от других (сигнатура формата).
  	  	  	  	  	  	  	//Может содержать единственное значение 4D4216/424D16 (little-endian/big-endian).
  uint32_t bfSize;  		//Размер файла в байтах.
  uint16_t bfReserved1;
  uint16_t bfReserved2;
  uint32_t bfOffBits;		//Положение пиксельных данных относительно начала данной структуры (в байтах).

  uint32_t bcSize;			//Размер данной структуры в байтах, указывающий также на версию структуры (здесь должно быть значение 12).
  uint32_t bcWidth;
  uint32_t bcHeight;
  uint16_t bcPlanes;
  uint16_t bcBitCount;
} BMP_TypeDef;


#endif
