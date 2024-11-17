#include "network.h"

#include <stdio.h>
#include <string.h>

static void wait_close()
{
    printf("\nPress any key to continue . . .");
    getchar();
}

int main(int argc, char** args)
{
    char address[19];
    if (argc == 1)
    {
        printf("Type an IPv4 address in the field below. It can include a subnet prefix.\n");
        printf("IP: ");
        if (fgets(address, 19, stdin) == 0)
        {
            printf("Error reading input.");
            wait_close();
            return 1;
        }
    }
    else
    {
        strncpy(address, args[1], 18);
    }

    int err;
    Network network;
    if (err = parse_address(address, &network))
    {
        if (err == INVALID_SUBNET)
        {
            printf("Invalid subnet: Network prefix cannot be less than class prefix\n");
            wait_close();
            return 0;
        }
        return err;
    }

    printf("\n");
    print_network_info(&network);
    wait_close();
    return 0;
}
