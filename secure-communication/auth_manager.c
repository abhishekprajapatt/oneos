#include "auth_manager.h"
#include <stdlib.h>
#include <string.h>

static AuthenticationManager *global_auth_manager = NULL;
static uint32_t next_user_id = 1;
static uint32_t next_role_id = 1;
static uint32_t next_token_id = 1;
static uint32_t next_session_id = 1;

AuthenticationManager *auth_manager_init(void)
{
    AuthenticationManager *manager = (AuthenticationManager *)malloc(sizeof(AuthenticationManager));
    if (!manager)
        return NULL;

    memset(manager, 0, sizeof(AuthenticationManager));
    manager->manager_id = 1;
    manager->user_count = 0;
    manager->role_count = 0;
    manager->token_count = 0;
    manager->session_count = 0;
    manager->successful_authentications = 0;
    manager->failed_authentications = 0;
    manager->active_sessions_count = 0;

    global_auth_manager = manager;
    return manager;
}

int auth_manager_destroy(AuthenticationManager *manager)
{
    if (!manager)
        return -1;

    free(manager);
    global_auth_manager = NULL;
    return 0;
}

uint32_t user_create(AuthenticationManager *manager, const char *username,
                     const char *email, const uint8_t *password_hash, uint32_t hash_length)
{
    if (!manager || manager->user_count >= MAX_AUTH_USERS || !username)
        return 0;

    User *user = &manager->users[manager->user_count];
    memset(user, 0, sizeof(User));

    user->user_id = next_user_id++;
    strncpy(user->username, username, sizeof(user->username) - 1);

    if (email)
    {
        strncpy(user->email, email, sizeof(user->email) - 1);
    }

    if (password_hash && hash_length <= 32)
    {
        memcpy(user->password_hash, password_hash, hash_length);
        user->password_hash_length = hash_length;
    }

    user->role_count = 0;
    user->created_time = 0;
    user->last_login_time = 0;
    user->is_active = 1;
    user->is_locked = 0;

    manager->user_count++;
    return user->user_id;
}

int user_delete(AuthenticationManager *manager, uint32_t user_id)
{
    if (!manager || user_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->user_count; i++)
    {
        if (manager->users[i].user_id == user_id)
        {
            memmove(&manager->users[i], &manager->users[i + 1],
                    sizeof(User) * (manager->user_count - i - 1));
            manager->user_count--;
            return 0;
        }
    }

    return -1;
}

int user_lock(AuthenticationManager *manager, uint32_t user_id)
{
    if (!manager || user_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->user_count; i++)
    {
        if (manager->users[i].user_id == user_id)
        {
            manager->users[i].is_locked = 1;
            return 0;
        }
    }

    return -1;
}

int user_unlock(AuthenticationManager *manager, uint32_t user_id)
{
    if (!manager || user_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->user_count; i++)
    {
        if (manager->users[i].user_id == user_id)
        {
            manager->users[i].is_locked = 0;
            return 0;
        }
    }

    return -1;
}

uint32_t role_create(AuthenticationManager *manager, const char *role_name)
{
    if (!manager || manager->role_count >= MAX_AUTH_ROLES || !role_name)
        return 0;

    Role *role = &manager->roles[manager->role_count];
    memset(role, 0, sizeof(Role));

    role->role_id = next_role_id++;
    strncpy(role->role_name, role_name, sizeof(role->role_name) - 1);
    role->permission_count = 0;
    role->created_time = 0;
    role->is_builtin = 0;

    manager->role_count++;
    return role->role_id;
}

int role_add_permission(AuthenticationManager *manager, uint32_t role_id, uint32_t permission)
{
    if (!manager || role_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->role_count; i++)
    {
        if (manager->roles[i].role_id == role_id)
        {
            Role *role = &manager->roles[i];

            if (role->permission_count < MAX_PERMISSIONS_PER_ROLE)
            {
                role->permissions[role->permission_count++] = permission;
                return 0;
            }

            return -1;
        }
    }

    return -1;
}

int role_remove_permission(AuthenticationManager *manager, uint32_t role_id, uint32_t permission)
{
    if (!manager || role_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->role_count; i++)
    {
        if (manager->roles[i].role_id == role_id)
        {
            Role *role = &manager->roles[i];

            for (uint32_t j = 0; j < role->permission_count; j++)
            {
                if (role->permissions[j] == permission)
                {
                    memmove(&role->permissions[j], &role->permissions[j + 1],
                            sizeof(uint32_t) * (role->permission_count - j - 1));
                    role->permission_count--;
                    return 0;
                }
            }

            return -1;
        }
    }

    return -1;
}

int user_assign_role(AuthenticationManager *manager, uint32_t user_id, uint32_t role_id)
{
    if (!manager || user_id == 0 || role_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->user_count; i++)
    {
        if (manager->users[i].user_id == user_id)
        {
            User *user = &manager->users[i];

            if (user->role_count < 64)
            {
                user->role_ids[user->role_count++] = role_id;
                return 0;
            }

            return -1;
        }
    }

    return -1;
}

int user_revoke_role(AuthenticationManager *manager, uint32_t user_id, uint32_t role_id)
{
    if (!manager || user_id == 0 || role_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->user_count; i++)
    {
        if (manager->users[i].user_id == user_id)
        {
            User *user = &manager->users[i];

            for (uint32_t j = 0; j < user->role_count; j++)
            {
                if (user->role_ids[j] == role_id)
                {
                    memmove(&user->role_ids[j], &user->role_ids[j + 1],
                            sizeof(uint32_t) * (user->role_count - j - 1));
                    user->role_count--;
                    return 0;
                }
            }

            return -1;
        }
    }

    return -1;
}

uint32_t auth_session_create(AuthenticationManager *manager, uint32_t user_id,
                             AuthenticationMethod method)
{
    if (!manager || manager->session_count >= MAX_AUTH_SESSIONS || user_id == 0)
        return 0;

    AuthenticationSession *session = &manager->sessions[manager->session_count];
    memset(session, 0, sizeof(AuthenticationSession));

    session->session_id = next_session_id++;
    session->user_id = user_id;
    session->auth_method = method;
    session->state = AUTH_STATE_UNAUTHENTICATED;
    session->session_start_time = 0;
    session->last_activity_time = 0;
    session->failed_auth_attempts = 0;
    session->mfa_enabled = 0;
    session->mfa_verified = 0;

    manager->session_count++;
    return session->session_id;
}

int auth_session_destroy(AuthenticationManager *manager, uint32_t session_id)
{
    if (!manager || session_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            if (manager->sessions[i].state == AUTH_STATE_AUTHENTICATED)
            {
                manager->active_sessions_count--;
            }

            memmove(&manager->sessions[i], &manager->sessions[i + 1],
                    sizeof(AuthenticationSession) * (manager->session_count - i - 1));
            manager->session_count--;
            return 0;
        }
    }

    return -1;
}

int auth_session_authenticate(AuthenticationManager *manager, uint32_t session_id,
                              const uint8_t *credentials, uint32_t cred_length)
{
    if (!manager || session_id == 0 || !credentials)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            AuthenticationSession *session = &manager->sessions[i];

            if (session->state == AUTH_STATE_UNAUTHENTICATED || session->state == AUTH_STATE_AUTHENTICATING)
            {
                session->state = AUTH_STATE_AUTHENTICATED;
                session->last_activity_time = 0;
                manager->successful_authentications++;
                manager->active_sessions_count++;
                return 0;
            }

            session->failed_auth_attempts++;
            manager->failed_authentications++;
            return -1;
        }
    }

    return -1;
}

int auth_session_verify_mfa(AuthenticationManager *manager, uint32_t session_id,
                            const uint8_t *mfa_code, uint32_t code_length)
{
    if (!manager || session_id == 0 || !mfa_code)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            AuthenticationSession *session = &manager->sessions[i];

            if (session->mfa_enabled)
            {
                session->mfa_verified = 1;
                return 0;
            }

            return -1;
        }
    }

    return -1;
}

uint32_t auth_token_generate(AuthenticationManager *manager, uint32_t user_id,
                             TokenType token_type)
{
    if (!manager || manager->token_count >= MAX_AUTH_TOKENS || user_id == 0)
        return 0;

    AuthenticationToken *token = &manager->tokens[manager->token_count];
    memset(token, 0, sizeof(AuthenticationToken));

    token->token_id = next_token_id++;
    token->user_id = user_id;
    token->token_type = token_type;
    token->issued_time = 0;
    token->expiry_time = 3600000;
    token->last_used_time = 0;
    token->is_valid = 1;
    token->usage_count = 0;

    snprintf(token->token_value, sizeof(token->token_value), "TOKEN_%u_%u", token->token_id, user_id);

    manager->token_count++;
    return token->token_id;
}

int auth_token_validate(AuthenticationManager *manager, uint32_t token_id)
{
    if (!manager || token_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->token_count; i++)
    {
        if (manager->tokens[i].token_id == token_id && manager->tokens[i].is_valid)
        {
            manager->tokens[i].last_used_time = 0;
            manager->tokens[i].usage_count++;
            return 0;
        }
    }

    return -1;
}

int auth_token_revoke(AuthenticationManager *manager, uint32_t token_id)
{
    if (!manager || token_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->token_count; i++)
    {
        if (manager->tokens[i].token_id == token_id)
        {
            manager->tokens[i].is_valid = 0;
            return 0;
        }
    }

    return -1;
}

int auth_verify_token(AuthenticationManager *manager, const char *token_value)
{
    if (!manager || !token_value)
        return -1;

    for (uint32_t i = 0; i < manager->token_count; i++)
    {
        if (manager->tokens[i].is_valid &&
            strcmp(manager->tokens[i].token_value, token_value) == 0)
        {
            return 0;
        }
    }

    return -1;
}

int auth_check_permission(AuthenticationManager *manager, uint32_t user_id,
                          uint32_t permission)
{
    if (!manager || user_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->user_count; i++)
    {
        if (manager->users[i].user_id == user_id)
        {
            User *user = &manager->users[i];

            for (uint32_t j = 0; j < user->role_count; j++)
            {
                for (uint32_t k = 0; k < manager->role_count; k++)
                {
                    if (manager->roles[k].role_id == user->role_ids[j])
                    {
                        for (uint32_t l = 0; l < manager->roles[k].permission_count; l++)
                        {
                            if (manager->roles[k].permissions[l] == permission)
                            {
                                return 0;
                            }
                        }
                    }
                }
            }

            return -1;
        }
    }

    return -1;
}

int auth_update_last_activity(AuthenticationManager *manager, uint32_t session_id)
{
    if (!manager || session_id == 0)
        return -1;

    for (uint32_t i = 0; i < manager->session_count; i++)
    {
        if (manager->sessions[i].session_id == session_id)
        {
            manager->sessions[i].last_activity_time = 0;
            return 0;
        }
    }

    return -1;
}

int auth_manager_get_metrics(AuthenticationManager *manager, AuthenticationManagerMetrics *metrics)
{
    if (!manager || !metrics)
        return -1;

    uint32_t active_users = 0;
    uint32_t locked_users = 0;
    uint32_t valid_tokens = 0;

    for (uint32_t i = 0; i < manager->user_count; i++)
    {
        if (manager->users[i].is_active)
        {
            active_users++;
        }
        if (manager->users[i].is_locked)
        {
            locked_users++;
        }
    }

    for (uint32_t i = 0; i < manager->token_count; i++)
    {
        if (manager->tokens[i].is_valid)
        {
            valid_tokens++;
        }
    }

    metrics->manager_id = manager->manager_id;
    metrics->total_users = manager->user_count;
    metrics->active_users = active_users;
    metrics->locked_users = locked_users;
    metrics->total_roles = manager->role_count;
    metrics->active_sessions = manager->active_sessions_count;
    metrics->valid_tokens = valid_tokens;
    metrics->successful_authentications = manager->successful_authentications;
    metrics->failed_authentications = manager->failed_authentications;

    return 0;
}
