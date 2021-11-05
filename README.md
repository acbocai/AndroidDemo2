# 声明:

该篇中涉及到的代码如未特别说明，均为本人自己所开发。该篇文档转载以及代码引用，需要注明出处

# 简介

​    该篇分享一个自己移植aosp7.1的CallStack栈回溯工具，该工具针对android5至android7有效。可以很好的帮助我们去分析app运行时，代码的调用上下文。

​    先看看回溯结果啥样子
###

```
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #00 pc 0000000000037961  /data/app/com.whulzz.demo-1/lib/x86_64/libdemo.so
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #01 pc 000000000001859b  /data/app/com.whulzz.demo-1/lib/x86_64/libdemo.so
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #02 pc 0000000000167391  /system/lib64/libart.so (art_quick_generic_jni_trampoline+209)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #03 pc 000000000015d146  /system/lib64/libart.so (art_quick_invoke_static_stub+806)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #04 pc 000000000016c604  /system/lib64/libart.so (_ZN3art9ArtMethod6InvokeEPNS_6ThreadEPjjPNS_6JValueEPKc+372)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #05 pc 00000000005a94e4  /system/lib64/libart.so (artInterpreterToCompiledCodeBridge+180)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #06 pc 0000000000329669  /system/lib64/libart.so (_ZN3art11interpreter6DoCallILb0ELb0EEEbPNS_9ArtMethodEPNS_6ThreadERNS_11ShadowFrameEPKNS_11InstructionEtPNS_6JValueE+457)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #07 pc 00000000003307cd  /system/lib64/libart.so (_ZN3art11interpreterL8DoInvokeILNS_10InvokeTypeE0ELb0ELb0EEEbPNS_6ThreadERNS_11ShadowFrameEPKNS_11InstructionEtPNS_6JValueE+141)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #08 pc 00000000001190e9  /system/lib64/libart.so (_ZN3art11interpreter15ExecuteGotoImplILb0ELb0EEENS_6JValueEPNS_6ThreadEPKNS_7DexFile8CodeItemERNS_11ShadowFrameES2_+23401)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #09 pc 00000000003035db  /system/lib64/libart.so (artInterpreterToInterpreterBridge+171)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #10 pc 0000000000329669  /system/lib64/libart.so (_ZN3art11interpreter6DoCallILb0ELb0EEEbPNS_9ArtMethodEPNS_6ThreadERNS_11ShadowFrameEPKNS_11InstructionEtPNS_6JValueE+457)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #11 pc 00000000003307cd  /system/lib64/libart.so (_ZN3art11interpreterL8DoInvokeILNS_10InvokeTypeE0ELb0ELb0EEEbPNS_6ThreadERNS_11ShadowFrameEPKNS_11InstructionEtPNS_6JValueE+141)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #12 pc 00000000001190e9  /system/lib64/libart.so (_ZN3art11interpreter15ExecuteGotoImplILb0ELb0EEENS_6JValueEPNS_6ThreadEPKNS_7DexFile8CodeItemERNS_11ShadowFrameES2_+23401)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #13 pc 00000000003035db  /system/lib64/libart.so (artInterpreterToInterpreterBridge+171)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #14 pc 0000000000329669  /system/lib64/libart.so (_ZN3art11interpreter6DoCallILb0ELb0EEEbPNS_9ArtMethodEPNS_6ThreadERNS_11ShadowFrameEPKNS_11InstructionEtPNS_6JValueE+457)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #15 pc 00000000003307cd  /system/lib64/libart.so (_ZN3art11interpreterL8DoInvokeILNS_10InvokeTypeE0ELb0ELb0EEEbPNS_6ThreadERNS_11ShadowFrameEPKNS_11InstructionEtPNS_6JValueE+141)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #16 pc 00000000001190e9  /system/lib64/libart.so (_ZN3art11interpreter15ExecuteGotoImplILb0ELb0EEENS_6JValueEPNS_6ThreadEPKNS_7DexFile8CodeItemERNS_11ShadowFrameES2_+23401)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #17 pc 0000000000303239  /system/lib64/libart.so (_ZN3art11interpreter30EnterInterpreterFromEntryPointEPNS_6ThreadEPKNS_7DexFile8CodeItemEPNS_11ShadowFrameE+89)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #18 pc 0000000000621d5a  /system/lib64/libart.so (artQuickToInterpreterBridge+506)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #19 pc 000000000016750c  /system/lib64/libart.so (art_quick_to_interpreter_bridge+140)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #20 pc 000000000002a2e3  /system/framework/x86_64/boot.oat (offset 0x1f57000)
```



​    如果你有以下场景，那么你可以试试该工具，非安卓人员可绕行

​    1.刚入职，如何更快的熟悉native-c代码的执行流程

​    2.分析某个开源的代码执行流程

​    3.运行时注入，查看代码调用上下文

​    针对上述场景，有时候采取源代码级别调试会比较简单。但是你总会遇到调试不友好(调试变卡，场景甚至无法触发)，此刻你不妨试试该调用栈回溯工具。

​    从此甩掉if语句+abort! 妈妈再也不用担心我无数次编译无数次崩溃了。

​    demo地址->[github](https://github.com/whulzz1993/AndroidDemo2)



# 该回溯工具用途/优劣势

## 常见用途

1.熟悉native-c代码执行流程

2.分析调用上下文

3.一些逆向分析场景

## 优:

1.方便实用，只需要对关注函数加上这一行代码

```c++
CallStack::log("您的TAG");//效果可查看[简介]->图片
```

2.基本不耗时，相对调试来讲不会造成卡顿甚至场景无法执行的情况

3.不会造成程序中断或者崩溃

## 劣:

目前只兼容android5-android7，安卓8以上解析不出来符号(有兴趣的可以尝试移植libunwindstack)



# 代码结构

## 关注重点123

```
admin@C02D7132MD6R Demo2 % ls -al
total 72
drwxr-xr-x  17 admin  staff   544 11  5 15:23 .
drwxr-xr-x  32 admin  staff  1024 11  5 12:29 ..
drwxr-xr-x  13 admin  staff   416 11  5 15:23 .git
-rw-r--r--   1 admin  staff   225 11  5 12:29 .gitignore
drwxr-xr-x   7 admin  staff   224 11  5 12:29 .gradle
drwxr-xr-x  13 admin  staff   416 11  5 15:23 .idea
-rw-r--r--   1 admin  staff   582 11  5 15:23 README
drwxr-xr-x   7 admin  staff   224 11  5 15:18 app ################################重点3
-rw-r--r--   1 admin  staff   530 11  5 12:29 build.gradle
drwxr-xr-x   3 admin  staff    96 11  5 12:29 gradle
-rw-r--r--   1 admin  staff  1093 11  5 12:29 gradle.properties
-rwxr--r--   1 admin  staff  5296 11  5 12:29 gradlew
-rw-r--r--   1 admin  staff  2260 11  5 12:29 gradlew.bat
drwxr-xr-x   5 admin  staff   160 11  5 12:47 include #####################################重点1
-rw-r--r--   1 admin  staff   434 11  5 12:29 local.properties
-rw-r--r--   1 admin  staff    41 11  5 12:29 settings.gradle
drwxr-xr-x   7 admin  staff   224 11  5 12:30 src ########################################重点2
admin@C02D7132MD6R Demo2 % 

```

## 1.include目录

### Include/base包含了常用的头文件

1.Scoepd 对c++编程比较熟悉的都知道Scoped，可有效防止内存忘记释放

2.ThreadPool.h 简易的一个线程池

3.ThreadUtil.h 线程相关的对象，线程销毁对象也会销毁

4.unique_fd.h 简易的管理文件描述符fd，配合使用std::unique_ptr有效的防止fd泄露

```
admin@C02D7132MD6R Demo2 % ls -al include/base 
total 56
drwxr-xr-x  9 admin  staff   288 11  5 13:21 .
drwxr-xr-x  5 admin  staff   160 11  5 12:47 ..
-rw-r--r--  1 admin  staff   899 11  5 12:44 ScopedBuffer.h
-rw-r--r--  1 admin  staff  1591 11  3 18:58 ScopedGuard.h
-rw-r--r--  1 admin  staff  1133 11  5 12:44 ScopedLocalUtfStr.h
-rw-r--r--  1 admin  staff  3673 11  5 12:48 ThreadPool.h
-rw-r--r--  1 admin  staff  1747 11  5 12:48 ThreadUtil.h
-rw-r--r--  1 admin  staff   715 11  5 12:48 log.h
-rw-r--r--  1 admin  staff   302 11  5 13:21 unique_fd.h
admin@C02D7132MD6R Demo2 % 
```

### Include/backtrace包含了回溯栈的头文件

```c++
class CallStack {
public:
    // Create a callstack with the current thread's stack trace.
    // Immediately dump it to logcat using the given logtag.
    static void log(const char* logtag);
};
```

### Include/json包含了json解析的头文件

移植自开源代码(https://github.com/open-source-parsers/jsoncpp)

如果代码中需要json解析，一般引入json.h就行了

```
admin@C02D7132MD6R Demo2 % ls -al include/json 
total 160
drwxr-xr-x  12 admin  staff    384 11  5 12:30 .
drwxr-xr-x   5 admin  staff    160 11  5 12:47 ..
-rw-r--r--   1 admin  staff   1789 11  5 12:30 assertions.h
-rw-r--r--   1 admin  staff    662 11  5 12:30 autolink.h
-rw-r--r--   1 admin  staff   4001 11  5 12:30 config.h
-rw-r--r--   1 admin  staff   1718 11  5 12:30 features.h
-rw-r--r--   1 admin  staff    938 11  5 12:30 forwards.h
-rw-r--r--   1 admin  staff    420 11  5 12:30 json.h
-rw-r--r--   1 admin  staff   8814 11  5 12:30 reader.h
-rw-r--r--   1 admin  staff  32733 11  5 12:30 value.h
-rw-r--r--   1 admin  staff    534 11  5 12:30 version.h
-rw-r--r--   1 admin  staff   6933 11  5 12:30 writer.h
admin@C02D7132MD6R Demo2 % 
```

## 2.src目录

Lib_json移植自(https://github.com/open-source-parsers/jsoncpp)

Libbacktrace/libbase/libunwind/lzma均移植自aosp7.1

```shell
admin@C02D7132MD6R Demo2 % ls -al src
total 0
drwxr-xr-x   7 admin  staff  224 11  5 12:30 .
drwxr-xr-x  17 admin  staff  544 11  5 15:23 ..
drwxr-xr-x  13 admin  staff  416 11  5 12:30 lib_json
drwxr-xr-x  24 admin  staff  768 11  5 12:30 libbacktrace
drwxr-xr-x  11 admin  staff  352 11  5 12:30 libbase
drwxr-xr-x   5 admin  staff  160 11  5 12:30 libunwind
drwxr-xr-x   3 admin  staff   96 11  5 12:30 lzma
```

### Lib_json使用(cmake)

```cmake
    add_subdirectory(${ROOT_DIR}/src/lib_json jsoncpp-bin)
    LIST(APPEND PROJECT_DEP_LIB jsoncpp_lib)
    add_definitions("-DENABLE_JSON_LIB")
```



```
admin@C02D7132MD6R Demo2 % ls -al src/lib_json
total 304
drwxr-xr-x  13 admin  staff    416 11  5 12:30 .
drwxr-xr-x   7 admin  staff    224 11  5 12:30 ..
-rw-r--r--   1 admin  staff   1640 11  5 12:30 CMakeLists.txt
-rw-r--r--   1 admin  staff   3950 11  5 12:30 json_batchallocator.h
-rw-r--r--   1 admin  staff  12377 11  5 12:30 json_internalarray.inl
-rw-r--r--   1 admin  staff  15558 11  5 12:30 json_internalmap.inl
-rw-r--r--   1 admin  staff  24841 11  5 12:30 json_reader.cpp
-rw-r--r--   1 admin  staff   2705 11  5 12:30 json_tool.h
-rw-r--r--   1 admin  staff  41461 11  5 12:30 json_value.cpp
-rw-r--r--   1 admin  staff   7403 11  5 12:30 json_valueiterator.inl
-rw-r--r--   1 admin  staff  19494 11  5 12:30 json_writer.cpp
-rw-r--r--   1 admin  staff    146 11  5 12:30 sconscript
-rw-r--r--   1 admin  staff    613 11  5 12:30 version.h.in
```





### libbacktrace使用(cmake)

```cmake
    add_subdirectory(${ROOT_DIR}/src/libbacktrace libbacktrace)
    LIST(APPEND PROJECT_DEP_LIB backtrace_lib)
```



```shell
admin@C02D7132MD6R Demo2 % ls -al src/libbacktrace 
total 208
drwxr-xr-x  24 admin  staff   768 11  5 12:30 .
drwxr-xr-x   7 admin  staff   224 11  5 12:30 ..
-rw-r--r--   1 admin  staff  4794 11  5 12:30 Backtrace.cpp
-rw-r--r--   1 admin  staff  6801 11  5 12:30 BacktraceCurrent.cpp
-rw-r--r--   1 admin  staff  1810 11  5 12:30 BacktraceCurrent.h
-rw-r--r--   1 admin  staff  1046 11  5 12:30 BacktraceLog.h
-rw-r--r--   1 admin  staff  4808 11  5 12:30 BacktraceMap.cpp
-rw-r--r--   1 admin  staff  2946 11  5 12:30 BacktraceOffline.h
-rw-r--r--   1 admin  staff  3213 11  5 12:30 BacktracePtrace.cpp
-rw-r--r--   1 admin  staff  1137 11  5 12:30 BacktracePtrace.h
-rw-r--r--   1 admin  staff  1519 11  5 12:30 CMakeLists.txt
-rw-r--r--   1 admin  staff  1135 11  5 12:30 CallStack.cpp
-rw-r--r--   1 admin  staff   743 11  5 12:30 GetPss.h
-rw-r--r--   1 admin  staff  3565 11  5 12:30 ThreadEntry.cpp
-rw-r--r--   1 admin  staff  1783 11  5 12:30 ThreadEntry.h
-rw-r--r--   1 admin  staff  4473 11  5 12:30 UnwindCurrent.cpp
-rw-r--r--   1 admin  staff  1418 11  5 12:30 UnwindCurrent.h
-rw-r--r--   1 admin  staff  4857 11  5 12:30 UnwindMap.cpp
-rw-r--r--   1 admin  staff  1796 11  5 12:30 UnwindMap.h
-rw-r--r--   1 admin  staff  3998 11  5 12:30 UnwindPtrace.cpp
-rw-r--r--   1 admin  staff  1270 11  5 12:30 UnwindPtrace.h
drwx------   3 admin  staff    96 11  5 12:30 include
-rw-r--r--   1 admin  staff   863 11  5 12:30 thread_utils.c
-rw-r--r--   1 admin  staff   890 11  5 12:30 thread_utils.h
```



# demo使用样例

app安装到手机上之后，点击snackbar按钮，logcat就会输出相关日志

## 关键代码:

```
using ClassFuncArray = std::map<std::string, std::unordered_set<std::string>>;
...
...
static void nativeTestJson(JNIEnv* env, jclass, jstring jpath) {
    ClassFuncArray funcArray = parseJsonFile(env, jpath);
    testJsonMethods(env, funcArray);
}

static void nativeTestUnw(JNIEnv* env, jclass) {
    CallStack::log("nativeTestUnw");
}

static const char *classPathName = "com/whulzz/demo/DemoApplication";

static JNINativeMethod methods[] = {
        {"testJson", "(Ljava/lang/String;)V", (void *) nativeTestJson},
        {"testUnw", "()V", (void*) nativeTestUnw}
};
```

## json解析日志

```
11-05 14:44:42.468  1732  1750 D LibEntry: json parse packages: [ com.whulzz.demo com.example.demo ]
11-05 14:44:42.468  1732  1750 D LibEntry: insert javaMethod: android.app.ApplicationPackageManager.getInstalledApplications
11-05 14:44:42.468  1732  1750 D LibEntry: insert javaMethod: android.app.ApplicationPackageManager.getInstalledPackages
11-05 14:44:42.468  1732  1750 D LibEntry: insert javaMethod: android.widget.Toast.show
11-05 14:44:42.468  1732  1750 D LibEntry: insert javaMethod: libcore.io.IoBridge.open
11-05 14:44:42.468  1732  1750 D LibEntry: parseConfigFile success
```

### 回溯栈日志

```
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #00 pc 0000000000037961  /data/app/com.whulzz.demo-1/lib/x86_64/libdemo.so
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #01 pc 000000000001859b  /data/app/com.whulzz.demo-1/lib/x86_64/libdemo.so
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #02 pc 0000000000167391  /system/lib64/libart.so (art_quick_generic_jni_trampoline+209)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #03 pc 000000000015d146  /system/lib64/libart.so (art_quick_invoke_static_stub+806)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #04 pc 000000000016c604  /system/lib64/libart.so (_ZN3art9ArtMethod6InvokeEPNS_6ThreadEPjjPNS_6JValueEPKc+372)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #05 pc 00000000005a94e4  /system/lib64/libart.so (artInterpreterToCompiledCodeBridge+180)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #06 pc 0000000000329669  /system/lib64/libart.so (_ZN3art11interpreter6DoCallILb0ELb0EEEbPNS_9ArtMethodEPNS_6ThreadERNS_11ShadowFrameEPKNS_11InstructionEtPNS_6JValueE+457)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #07 pc 00000000003307cd  /system/lib64/libart.so (_ZN3art11interpreterL8DoInvokeILNS_10InvokeTypeE0ELb0ELb0EEEbPNS_6ThreadERNS_11ShadowFrameEPKNS_11InstructionEtPNS_6JValueE+141)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #08 pc 00000000001190e9  /system/lib64/libart.so (_ZN3art11interpreter15ExecuteGotoImplILb0ELb0EEENS_6JValueEPNS_6ThreadEPKNS_7DexFile8CodeItemERNS_11ShadowFrameES2_+23401)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #09 pc 00000000003035db  /system/lib64/libart.so (artInterpreterToInterpreterBridge+171)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #10 pc 0000000000329669  /system/lib64/libart.so (_ZN3art11interpreter6DoCallILb0ELb0EEEbPNS_9ArtMethodEPNS_6ThreadERNS_11ShadowFrameEPKNS_11InstructionEtPNS_6JValueE+457)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #11 pc 00000000003307cd  /system/lib64/libart.so (_ZN3art11interpreterL8DoInvokeILNS_10InvokeTypeE0ELb0ELb0EEEbPNS_6ThreadERNS_11ShadowFrameEPKNS_11InstructionEtPNS_6JValueE+141)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #12 pc 00000000001190e9  /system/lib64/libart.so (_ZN3art11interpreter15ExecuteGotoImplILb0ELb0EEENS_6JValueEPNS_6ThreadEPKNS_7DexFile8CodeItemERNS_11ShadowFrameES2_+23401)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #13 pc 00000000003035db  /system/lib64/libart.so (artInterpreterToInterpreterBridge+171)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #14 pc 0000000000329669  /system/lib64/libart.so (_ZN3art11interpreter6DoCallILb0ELb0EEEbPNS_9ArtMethodEPNS_6ThreadERNS_11ShadowFrameEPKNS_11InstructionEtPNS_6JValueE+457)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #15 pc 00000000003307cd  /system/lib64/libart.so (_ZN3art11interpreterL8DoInvokeILNS_10InvokeTypeE0ELb0ELb0EEEbPNS_6ThreadERNS_11ShadowFrameEPKNS_11InstructionEtPNS_6JValueE+141)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #16 pc 00000000001190e9  /system/lib64/libart.so (_ZN3art11interpreter15ExecuteGotoImplILb0ELb0EEENS_6JValueEPNS_6ThreadEPKNS_7DexFile8CodeItemERNS_11ShadowFrameES2_+23401)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #17 pc 0000000000303239  /system/lib64/libart.so (_ZN3art11interpreter30EnterInterpreterFromEntryPointEPNS_6ThreadEPKNS_7DexFile8CodeItemEPNS_11ShadowFrameE+89)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #18 pc 0000000000621d5a  /system/lib64/libart.so (artQuickToInterpreterBridge+506)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #19 pc 000000000016750c  /system/lib64/libart.so (art_quick_to_interpreter_bridge+140)
11-05 14:44:42.490  1732  1750 D nativeTestUnw: #20 pc 000000000002a2e3  /system/framework/x86_64/boot.oat (offset 0x1f57000)
```

