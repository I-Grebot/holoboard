/* -----------------------------------------------------------------------------
 * BlueBoard
 * I-Grebot 2016
 * -----------------------------------------------------------------------------
 * @file       syscalls.c
 * @author     Paul
 * @date       Jan 3, 2016
 * @version    V1.0
 * -----------------------------------------------------------------------------
 * @brief
 *   System call stub to route them to hardware
 * -----------------------------------------------------------------------------
 * Versionning informations
 * Repository: https://github.com/I-Grebot/blueboard.git
 * -----------------------------------------------------------------------------
 */

/* Includes */
#include "main.h"

int _write(int file, char *ptr, int len)
{
    int DataIdx;

    for (DataIdx = 0; DataIdx < len; DataIdx++)
    {
        serial_put((*ptr++));
    }
    return len;
}
