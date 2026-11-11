#pragma once

#include "../model/User.h"
#include "../service/UserService.h"

#include <string>
#include <drogon/drogon.h>

class UserController : public drogon::HttpController<UserController>
{
private:
    UserService userService;

public:
    // =========================
    // HTTP 接口
    // =========================

    METHOD_LIST_BEGIN

    // 注册
    ADD_METHOD_TO(
        UserController::registerUser,
        "/api/register",
        drogon::Post
    );

    // 登录
    ADD_METHOD_TO(
        UserController::loginHttp,
        "/api/login",
        drogon::Post
    );

    // 查询用户
    ADD_METHOD_TO(
        UserController::getUserHttp,
        "/api/users/{1}",
        drogon::Get
    );

    // 修改用户
    ADD_METHOD_TO(
        UserController::updateUserHttp,
        "/api/users/{1}",
        drogon::Put
    );

    METHOD_LIST_END


    // =========================
    // HTTP Handler
    // =========================

    void registerUser(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    void loginHttp(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    void getUserHttp(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

    void updateUserHttp(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );


    // =========================
    // 原来的控制台接口
    // =========================

    bool loginUser(
        const std::string& username,
        const std::string& password
    );

    bool getUserById(
        int id,
        User& user
    );

    bool updateUser(
        const User& user
    );
};