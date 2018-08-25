#ifndef DUKTAPE_HELPER_HPP
#define DUKTAPE_HELPER_HPP

#include <memory>
#include <string>
#include <type_traits>
#include "duktape.h"

namespace duktape {

    //duk get result helper
    template< typename T >
    struct duk_result
    {
        static T get(duk_context *ctx, duk_idx_t idx)
        {
            return duk_to_null(ctx,idx);
        }
    };
    template<>
    struct duk_result<int>
    {
        static int get(duk_context *ctx, duk_idx_t idx)
        {
            return duk_to_int(ctx,idx);
        }
    };

    template<>
    struct duk_result<double>
    {
        static double get(duk_context *ctx, duk_idx_t idx)
        {
            return duk_to_number(ctx,idx);
        }
    };

    template<>
    struct duk_result<const char*>
    {
        static const char* get(duk_context *ctx, duk_idx_t idx)
        {
            return duk_to_string(ctx,idx);
        }
    };
    template<>
    struct duk_result<bool>
    {
        static bool get(duk_context *ctx, duk_idx_t idx)
        {
            return duk_to_boolean(ctx,idx);
        }
    };

    class ContextPtr
    {
        using Deleter = void (*)(duk_context *);
        std::unique_ptr<duk_context,Deleter> ptr_;
        ContextPtr( const ContextPtr &) = delete;
        ContextPtr& operator = ( const ContextPtr&) = delete;
    public:
        ContextPtr(duk_context *p ):ptr_(p,duk_destroy_heap)
        {}
        inline operator duk_context *() noexcept
        {
            return ptr_.get();
        }
        inline operator duk_context *() const noexcept
        {
            return ptr_.get();
        }
    };

    using deleter = void(*)(duk_context*);
    typedef std::unique_ptr<duk_context,deleter> stack_pop;

    class script
    {

    public:
        script():m_ctx(duk_create_heap_default())
        {

        }

        duk_context * context() const
        {
            return m_ctx;
        }
        inline operator duk_context *() noexcept
        {
            return m_ctx;
        }
        inline operator duk_context *() const noexcept
        {
            return m_ctx;
        }

        void evaluate( const std::string & script )
        {
            stack_pop pop(m_ctx,duk_pop);
            duk_eval_string(m_ctx,script.c_str());
        }

        template<typename R , typename ...Args>
        R evaluate(const char* functionName ,Args ...args )
        {
            stack_pop pop(m_ctx,duk_pop);
            duk_get_global_string(m_ctx,functionName);
            duk_push_args(m_ctx,args...);
            duk_call(m_ctx,sizeof...(args));

           return duk_result<R>::get(m_ctx,-1);
        }

        void defineFunctions(duk_function_list_entry * fs )
        {
            for( ; fs->key ; fs++ ){
                duk_push_c_function(m_ctx, fs->value, fs->nargs);
                duk_put_global_string(m_ctx, fs->key);
            }
        }
        void defineFunction(const char* key , duk_c_function func, duk_idx_t nargs)
        {
            duk_push_c_function(m_ctx, func, nargs);
            duk_put_global_string(m_ctx, key);
        }

    private:
        ContextPtr m_ctx;
        script( const script &) = delete;
        script& operator = ( const script&) = delete;

    };

    //help template function for duk_push_
    template<typename T> void duk_push_arg(duk_context *ctx,T t) {}
    template<> void duk_push_arg(duk_context *ctx,int t)
    {
        duk_push_int(ctx,t);
    }
    template<> void duk_push_arg(duk_context *ctx,double t)
    {
        duk_push_number(ctx,t);
    }
    template<> void duk_push_arg(duk_context *ctx,const char* t)
    {
        duk_push_string(ctx,t);
    }
    template<> void duk_push_arg(duk_context *ctx,bool t)
    {
        duk_push_boolean(ctx,t);
    }
    template<> void duk_push_arg(duk_context *ctx,void* t)
    {
        duk_push_pointer(ctx,t);
    }
    template <typename ...Args>
    void duk_push_args(duk_context *ctx,Args... args)
    {
        std::initializer_list<int> lst{(duk_push_arg(ctx,std::forward< Args>(args)),0)...};
    }

}//namespace Duktape

#endif // DUKTAPE_HELPER_HPP
