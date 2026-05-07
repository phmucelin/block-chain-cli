#include <unity.h>
#include "services/user_actions.c"
void test_uuid_not_null(void)
{
    char* id = generate_uuid();
    TEST_ASSERT_GENERATE_UUID();
}