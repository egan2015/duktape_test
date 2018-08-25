#include <iostream>
#include <memory>

#include "duktape_helper.hpp"

static duk_ret_t native_print(duk_context *ctx) {
    duk_push_string(ctx, " ");
    duk_insert(ctx, 0);
    duk_join(ctx, duk_get_top(ctx) - 1);
    std::cout<<duk_safe_to_string(ctx, -1)<<std::endl;
    return 0;
}

static duk_ret_t native_adder(duk_context *ctx) {
    int i;
    int n = duk_get_top(ctx);  /* #args */
    double res = 0.0;

    for (i = 0; i < n; i++) {
        res += duk_to_number(ctx, i);
    }
    duk_push_number(ctx, res);
    return 1;  /* one return value */
}


int main()
{
    std::cout << "Run duktape JavaScript !" << std::endl;

    std::shared_ptr<duktape::script> duk = std::make_shared<duktape::script>();

    duk_function_list_entry functions[] = {
        {"print",native_print,DUK_VARARGS},
        {"adder",native_adder,DUK_VARARGS},
        {0}
    };

    duk->defineFunctions(functions);

    duk->evaluate("var a = 'Hello world'; print(a + ' ' + Duktape.version);");
    duk->evaluate("print('2.2+3.3=' + adder(2.2, 3.3));");
    duk->evaluate("function sum(a,b){ return a+b; }");
    duk->evaluate("var printVersion = function (msg,a,b){"
                  "var disp = msg+(a+b) ; "
                  "print ('Duktape Version: '+ Duktape.version); "
                  "return disp;}");

    const char* result;
    result = duk->evaluate<const char*>("printVersion","call javascript function ",56,44);
    std::cout<<result<<std::endl;
    double ret;
    ret = duk->evaluate<double>("sum",56,44);
    std::cout<<ret<<std::endl;

    return 0;
}
