#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_AUTH_SESSIONS 1024
#define MAX_AUTH_TOKENS 2048
#define MAX_AUTH_USERS 512
#define MAX_AUTH_ROLES 128
#define MAX_PERMISSIONS_PER_ROLE 256

typedef enum
{
    AUTH_METHOD_PASSWORD,
    AUTH_METHOD_CERTIFICATE,
    AUTH_METHOD_OAUTH2,
    AUTH_METHOD_SAML,
    AUTH_METHOD_MFA,
    AUTH_METHOD_BIOMETRIC
} AuthenticationMethod;

typedef enum
{
    TOKEN_TYPE_ACCESS,
    TOKEN_TYPE_REFRESH,
    TOKEN_TYPE_ID_TOKEN,
    TOKEN_TYPE_SERVICE_ACCOUNT
} TokenType;

typedef enum
{
    AUTH_STATE_UNAUTHENTICATED,
    AUTH_STATE_AUTHENTICATING,
    AUTH_STATE_AUTHENTICATED,
    AUTH_STATE_EXPIRED,
    AUTH_STATE_REVOKED
} AuthenticationState;

typedef struct
{
    uint32_t role_id;
    char role_name[64];

    uint32_t permissions[MAX_PERMISSIONS_PER_ROLE];
    uint32_t permission_count;

    uint64_t created_time;
    uint8_t is_builtin;
} Role;

typedef struct
{
    uint32_t user_id;
    char username[128];
    char email[256];

    uint8_t password_hash[32];
    uint32_t password_hash_length;

    uint32_t role_ids[64];
    uint32_t role_count;

    uint64_t created_time;
    uint64_t last_login_time;
    uint8_t is_active;
    uint8_t is_locked;
} User;

typedef struct
{
    uint32_t token_id;
    char token_value[512];

    uint32_t user_id;
    TokenType token_type;

    uint64_t issued_time;
    uint64_t expiry_time;
    uint64_t last_used_time;

    uint8_t is_valid;
    uint32_t usage_count;
} AuthenticationToken;

typedef struct
{
    uint32_t session_id;
    uint32_t user_id;

    AuthenticationMethod auth_method;
    AuthenticationState state;

    uint32_t active_token_id;
    uint32_t refresh_token_id;

    uint64_t session_start_time;
    uint64_t last_activity_time;
    uint32_t failed_auth_attempts;

    uint8_t mfa_enabled;
    uint8_t mfa_verified;
} AuthenticationSession;

typedef struct
{
    uint32_t manager_id;

    User users[MAX_AUTH_USERS];
    uint32_t user_count;

    Role roles[MAX_AUTH_ROLES];
    uint32_t role_count;

    AuthenticationToken tokens[MAX_AUTH_TOKENS];
    uint32_t token_count;

    AuthenticationSession sessions[MAX_AUTH_SESSIONS];
    uint32_t session_count;

    uint64_t successful_authentications;
    uint64_t failed_authentications;
    uint32_t active_sessions_count;
} AuthenticationManager;

AuthenticationManager *auth_manager_init(void);
int auth_manager_destroy(AuthenticationManager *manager);

uint32_t user_create(AuthenticationManager *manager, const char *username,
                     const char *email, const uint8_t *password_hash, uint32_t hash_length);
int user_delete(AuthenticationManager *manager, uint32_t user_id);
int user_lock(AuthenticationManager *manager, uint32_t user_id);
int user_unlock(AuthenticationManager *manager, uint32_t user_id);

uint32_t role_create(AuthenticationManager *manager, const char *role_name);
int role_add_permission(AuthenticationManager *manager, uint32_t role_id, uint32_t permission);
int role_remove_permission(AuthenticationManager *manager, uint32_t role_id, uint32_t permission);

int user_assign_role(AuthenticationManager *manager, uint32_t user_id, uint32_t role_id);
int user_revoke_role(AuthenticationManager *manager, uint32_t user_id, uint32_t role_id);

uint32_t auth_session_create(AuthenticationManager *manager, uint32_t user_id,
                             AuthenticationMethod method);
int auth_session_destroy(AuthenticationManager *manager, uint32_t session_id);

int auth_session_authenticate(AuthenticationManager *manager, uint32_t session_id,
                              const uint8_t *credentials, uint32_t cred_length);
int auth_session_verify_mfa(AuthenticationManager *manager, uint32_t session_id,
                            const uint8_t *mfa_code, uint32_t code_length);

uint32_t auth_token_generate(AuthenticationManager *manager, uint32_t user_id,
                             TokenType token_type);
int auth_token_validate(AuthenticationManager *manager, uint32_t token_id);
int auth_token_revoke(AuthenticationManager *manager, uint32_t token_id);

int auth_verify_token(AuthenticationManager *manager, const char *token_value);

int auth_check_permission(AuthenticationManager *manager, uint32_t user_id,
                          uint32_t permission);

int auth_update_last_activity(AuthenticationManager *manager, uint32_t session_id);

typedef struct
{
    uint32_t manager_id;
    uint32_t total_users;
    uint32_t active_users;
    uint32_t locked_users;
    uint32_t total_roles;
    uint32_t active_sessions;
    uint32_t valid_tokens;
    uint64_t successful_authentications;
    uint64_t failed_authentications;
} AuthenticationManagerMetrics;

int auth_manager_get_metrics(AuthenticationManager *manager, AuthenticationManagerMetrics *metrics);

#endif
