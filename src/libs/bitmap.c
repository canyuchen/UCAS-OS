#include "bitmap.h"

int check_bitmap(BitMap_t bitmap, unsigned int index) {
    
    if (bitmap == NULL) {

    }
    
    unsigned int byte =  (index / 8);
    uint8_t offset = (index % 8);
    
    uint8_t shift = (0x1 << offset) & bitmap[byte];
    return (shift != 0);
}

int set_bitmap(BitMap_t bitmap, unsigned int index) {
    
    if (bitmap == NULL) {

    }
    
    unsigned int byte =  (index / 8);
    uint8_t offset = (index % 8);
    
    bitmap[byte] |= (0x1 << offset);
    
    return 0;
}

int unset_bitmap(BitMap_t bitmap, unsigned int index) {
    if (bitmap == NULL) {

    }
    
    unsigned int byte =  index / 8;
    uint8_t offset = index % 8;
    
    bitmap[byte] &= (~(0x1 << offset));
    
    return 0;
}