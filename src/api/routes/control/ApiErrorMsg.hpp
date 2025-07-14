#pragma once

#define __MSG inline static std::string

class ApiErrorMsg {

public:
  __MSG NO_CREDENTIALS = "no authorization header";
  __MSG INVALID_CREDENTIALS = "invalid credentials";
  __MSG FORBIDDEN_ACCESS = "forbidden access";
  __MSG TOKEN_EXPIRED = "token expired";
};
