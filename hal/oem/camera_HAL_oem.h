#ifndef _CAMERA_HAL_H_
#define _CAMERA_HAL_H_

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

#include <stdint.h>

int oem_camera_open(void);
int oem_camera_take_picture(void);
int oem_camera_dump(void);

#ifdef __cplusplus
} // extern "C"
#endif  // __cplusplus

#endif /* _CAMERA_HAL_H_ */

