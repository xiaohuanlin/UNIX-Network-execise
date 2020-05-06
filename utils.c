#include <stdio.h>
#include <stdint.h>
#include <string.h>


// Convert the string ip address to uint32 number. Negtive number means error
uint32_t transfer_ip(const char *ip_address) {
    uint8_t ip_value[4] = {};
    int current_ip_slice = 0;
    char current_ip_cahr;

    int i;
    for (i = 0; i < strlen(ip_address); i++) {
        if ((current_ip_cahr = ip_address[i]) == '.') {
            current_ip_slice += 1;
            if (*ip_value > 255) {
                // overflow
                return -1;
            }
            continue;
        }
        ip_value[current_ip_slice] = ip_value[current_ip_slice]*10 + (current_ip_cahr - '0');
    }

    if (current_ip_slice != 3) {
        return -1;
    }

    uint32_t true_value = ip_value[0] << 24 | ip_value[1] << 16 | ip_value[2] << 8 | ip_value[3];
    return true_value;
}