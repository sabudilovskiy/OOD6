//
// Created by Andrei Budilovskii on 14.03.2023.
//
#include "Handler.hpp"
#include <iostream>
#include "System.hpp"
#include <ranges>
#include <sstream>
#include <vector>
#include <format>

struct AbstractHandler{
    std::function<bool(std::string_view)> next;
    virtual bool operator()(std::string_view path) = 0;
    template<typename Callable>
    void SetNext(Callable&& callable){
        next = decltype(next){callable};
    }
    AbstractHandler(){
        next = [](std::string_view){return false;};
    }
    virtual ~AbstractHandler(){}
};

std::vector<std::string> FindApp(std::string_view name){
    auto cmd = std::string("whereis ").append(name);
    std::stringstream ss(OS::exec(cmd));
    std::vector<std::string> result;
    std::string temp;
    ss >> temp;
    while (!ss.eof()){
        ss >> temp;
        if (!ss.fail() && !temp.empty()) result.emplace_back(std::move(temp));
    }
    return result;
}


template<typename Derived>
struct BaseHandler : AbstractHandler{
    bool operator()(std::string_view path) override {
        bool result = static_cast<Derived*>(this)->DoHandle(path);
        if (!result){
            result = next(path);
        }
        return result;
    }
};

struct JavaHandler: BaseHandler<JavaHandler>{
    bool RunJavaApp(std::string_view path_java, std::string_view path_file){
        try {
            std::string cmd{path_java};
            cmd.append(" -jar ").append(path_file);
            std::cout << OS::exec(cmd);
        }
        catch (std::exception& exc){
            std::cout << "Error: " << exc.what() << '\n';
        }
    }
    bool DoHandle(std::string_view path){
        if (!path.ends_with(".jar")) return false;
        std::cout << "Java application. Try to find Java\n";
        auto paths = FindApp("java");
        if (paths.empty()){
            return false;
        }
        for (auto& jar_path: paths){
            try {
                RunJavaApp(jar_path, path);
                return true;
            }
            catch (std::exception& exc){
                std::cout << "Error from " << jar_path << ": " << exc.what() << '\n';
            }
        }
        return false;
    }
};

struct PythonHandler: BaseHandler<PythonHandler>{
    bool RunPythonApp(std::string_view path_python, std::string_view path_file){
        try {
            std::string cmd{path_python};
            cmd.append(" ").append(path_file);
            std::cout << OS::exec(cmd);
        }
        catch (std::exception& exc){
            std::cout << "Error: " << exc.what() << '\n';
        }
    }
    bool DoHandle(std::string_view path){
        if (!path.ends_with(".py")) return false;
        std::cout << "Python script. Try to find Python\n";
        auto paths = FindApp("python3");
        if (paths.empty()){
            return false;
        }
        for (auto& py_path: paths){
            try {
                RunPythonApp(py_path, path);
                return true;
            }
            catch (std::exception& exc){
                std::cout << "Error from " << py_path << ": " << exc.what() << '\n';
            }
        }
        return false;
    }
};

std::function<bool(std::string_view)> CreateAll(){
    JavaHandler javaHandler;
    javaHandler.SetNext(PythonHandler{});
    return javaHandler;
}