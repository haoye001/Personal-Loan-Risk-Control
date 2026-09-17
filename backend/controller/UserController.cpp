#include "UserController.h"

#include <iostream>


// ============================================================
// POST /api/register
// 用户注册
// ============================================================

void UserController::registerUser(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto json = req->getJsonObject();

    // JSON 解析失败
    if (!json)
    {
        Json::Value result;

        result["success"] = false;
        result["message"] = "invalid json";

        auto resp =
            drogon::HttpResponse::newHttpJsonResponse(result);

        resp->setStatusCode(drogon::k400BadRequest);

        callback(resp);
        return;
    }

    // 构造 User
    User user;

    user.username = (*json)["username"].asString();
    user.password = (*json)["password"].asString();
    user.phone = (*json)["phone"].asString();

    // 调用 Service
    if (userService.registerUser(user))
    {
        Json::Value result;

        result["success"] = true;
        result["message"] = "register success";

        auto resp =
            drogon::HttpResponse::newHttpJsonResponse(result);

        resp->setStatusCode(drogon::k200OK);

        callback(resp);
        return;
    }

    // 用户名已经存在
    Json::Value result;

    result["success"] = false;
    result["message"] = "username already exists";

    auto resp =
        drogon::HttpResponse::newHttpJsonResponse(result);

    resp->setStatusCode(drogon::k409Conflict);

    callback(resp);
}


// ============================================================
// POST /api/login
// 用户登录
// ============================================================

void UserController::loginHttp(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback)
{
    auto json = req->getJsonObject();

    // JSON 解析失败
    if (!json)
    {
        Json::Value result;

        result["success"] = false;
        result["message"] = "invalid json";

        auto resp =
            drogon::HttpResponse::newHttpJsonResponse(result);

        resp->setStatusCode(drogon::k400BadRequest);

        callback(resp);
        return;
    }

    std::string username =
        (*json)["username"].asString();

    std::string password =
        (*json)["password"].asString();

    // 调用 Service
    if (userService.loginUser(username, password))
    {
        Json::Value result;

        result["success"] = true;
        result["message"] = "login success";

        auto resp =
            drogon::HttpResponse::newHttpJsonResponse(result);

        resp->setStatusCode(drogon::k200OK);

        callback(resp);
        return;
    }

    // 登录失败
    Json::Value result;

    result["success"] = false;
    result["message"] = "username or password error";

    auto resp =
        drogon::HttpResponse::newHttpJsonResponse(result);

    resp->setStatusCode(drogon::k401Unauthorized);

    callback(resp);
}


// ============================================================
// GET /api/users/{id}
// 查询用户
// ============================================================

void UserController::getUserHttp(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int id)
{
    std::cout << "GET /api/users/{id} entered, id = "
              << id << std::endl;
    User user;

    // 调用 Service 查询用户
    if (userService.getUserById(id, user))
    {
        Json::Value result;

        result["success"] = true;

        result["id"] = user.id;
        result["username"] = user.username;
        result["phone"] = user.phone;

        // 注意：
        // 不返回 password

        auto resp =
            drogon::HttpResponse::newHttpJsonResponse(result);

        resp->setStatusCode(drogon::k200OK);

        callback(resp);
        return;
    }

    // 用户不存在
    Json::Value result;

    result["success"] = false;
    result["message"] = "user not found";

    auto resp =
        drogon::HttpResponse::newHttpJsonResponse(result);

    resp->setStatusCode(drogon::k404NotFound);

    callback(resp);
}


// ============================================================
// PUT /api/users/{id}
// 修改用户
// ============================================================

void UserController::updateUserHttp(
    const drogon::HttpRequestPtr& req,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int id)
{
    // 获取 JSON 请求体
    auto json = req->getJsonObject();

    // JSON 解析失败
    if (!json)
    {
        Json::Value result;

        result["success"] = false;
        result["message"] = "invalid json";

        auto resp =
            drogon::HttpResponse::newHttpJsonResponse(result);

        resp->setStatusCode(drogon::k400BadRequest);

        callback(resp);
        return;
    }

    // 构造 User
    User user;

    // id 来自 URL：
    // PUT /api/users/1
    //                  ↑
    //                  id = 1
    user.id = id;

    user.username =
        (*json)["username"].asString();

    user.password =
        (*json)["password"].asString();

    user.phone =
        (*json)["phone"].asString();

    // 调用 Service
    if (userService.updateUser(user))
    {
        Json::Value result;

        result["success"] = true;
        result["message"] = "user update success";

        auto resp =
            drogon::HttpResponse::newHttpJsonResponse(result);

        resp->setStatusCode(drogon::k200OK);

        callback(resp);
        return;
    }

    // 修改失败
    Json::Value result;

    result["success"] = false;
    result["message"] = "user update failed";

    auto resp =
        drogon::HttpResponse::newHttpJsonResponse(result);

    resp->setStatusCode(drogon::k400BadRequest);

    callback(resp);
}


// ============================================================
// 原来的控制台登录接口
// ============================================================

bool UserController::loginUser(
    const std::string& username,
    const std::string& password)
{
    if (userService.loginUser(username, password))
    {
        std::cout
            << "Controller: Login success"
            << std::endl;

        return true;
    }

    std::cout
        << "Controller: Login failed"
        << std::endl;

    return false;
}


// ============================================================
// 原来的控制台查询接口
// ============================================================

bool UserController::getUserById(
    int id,
    User& user)
{
    if (userService.getUserById(id, user))
    {
        std::cout
            << "Controller: Get user success"
            << std::endl;

        std::cout
            << "id: "
            << user.id
            << std::endl;

        std::cout
            << "username: "
            << user.username
            << std::endl;

        std::cout
            << "phone: "
            << user.phone
            << std::endl;

        return true;
    }

    std::cout
        << "Controller: User not found"
        << std::endl;

    return false;
}


// ============================================================
// 原来的控制台修改接口
// ============================================================

bool UserController::updateUser(
    const User& user)
{
    if (userService.updateUser(user))
    {
        std::cout
            << "Controller: user update success"
            << std::endl;

        return true;
    }

    std::cout
        << "Controller: user update failed"
        << std::endl;

    return false;
}