#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int po_create_optimizer(const char *name);
int arm_create_manager(const char *name);
int shm_create_manager(const char *name);
int ic_create_coordinator(const char *name);

int test_predictive_optimizer(void);
int test_autonomous_resource_manager(void);
int test_self_healing_manager(void);
int test_intelligence_coordinator(void);

int main(void)
{
    printf("========================================\n");
    printf("  OneOS Part 12 Subsystems Test Suite\n");
    printf("========================================\n\n");

    int total_passed = 0;
    int total_failed = 0;

    printf("[1/4] Testing Predictive Optimizer...\n");
    if (test_predictive_optimizer() == 0)
    {
        printf("  ✓ PASSED\n");
        total_passed++;
    }
    else
    {
        printf("  ✗ FAILED\n");
        total_failed++;
    }

    printf("[2/4] Testing Autonomous Resource Manager...\n");
    if (test_autonomous_resource_manager() == 0)
    {
        printf("  ✓ PASSED\n");
        total_passed++;
    }
    else
    {
        printf("  ✗ FAILED\n");
        total_failed++;
    }

    printf("[3/4] Testing Self-Healing Manager...\n");
    if (test_self_healing_manager() == 0)
    {
        printf("  ✓ PASSED\n");
        total_passed++;
    }
    else
    {
        printf("  ✗ FAILED\n");
        total_failed++;
    }

    printf("[4/4] Testing Intelligence Coordinator...\n");
    if (test_intelligence_coordinator() == 0)
    {
        printf("  ✓ PASSED\n");
        total_passed++;
    }
    else
    {
        printf("  ✗ FAILED\n");
        total_failed++;
    }

    printf("\n========================================\n");
    printf("  Results: %d PASSED, %d FAILED\n", total_passed, total_failed);
    printf("========================================\n");

    return (total_failed > 0) ? 1 : 0;
}

int test_predictive_optimizer(void)
{
    printf("  - Creating predictive optimizer instance...");
    int opt_id = po_create_optimizer("TestOptimizer");
    if (opt_id <= 0)
    {
        printf(" FAILED\n");
        return -1;
    }
    printf(" OK (ID: %d)\n", opt_id);
    return 0;
}

int test_autonomous_resource_manager(void)
{
    printf("  - Creating resource manager instance...");
    int mgr_id = arm_create_manager("TestResourceManager");
    if (mgr_id <= 0)
    {
        printf(" FAILED\n");
        return -1;
    }
    printf(" OK (ID: %d)\n", mgr_id);
    return 0;
}

int test_self_healing_manager(void)
{
    printf("  - Creating self-healing manager instance...");
    int shm_id = shm_create_manager("TestSelfHealingMgr");
    if (shm_id <= 0)
    {
        printf(" FAILED\n");
        return -1;
    }
    printf(" OK (ID: %d)\n", shm_id);
    return 0;
}

int test_intelligence_coordinator(void)
{
    printf("  - Creating intelligence coordinator instance...");
    int ic_id = ic_create_coordinator("TestIntelligenceCoordinator");
    if (ic_id <= 0)
    {
        printf(" FAILED\n");
        return -1;
    }
    printf(" OK (ID: %d)\n", ic_id);
    return 0;
}
