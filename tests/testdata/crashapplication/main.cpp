#include <string.h>

int main()
{
    int *p1 = NULL;
    // coverity[var_deref_op]
    *p1 = 1;
}
