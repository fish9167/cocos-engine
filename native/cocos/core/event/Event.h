/****************************************************************************
 Copyright (c) 2022 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos.com
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated engine source code (the "Software"), a limited,
 worldwide, royalty-free, non-assignable, revocable and non-exclusive license
 to use Cocos Creator solely to develop games on your target platforms. You shall
 not use Cocos Creator software for developing other software or tools that's
 used for developing games. You are not granted to publish, distribute,
 sublicense, and/or sell copies of Cocos Creator.
 
 The software or tools in this License Agreement are licensed, not sold.
 Xiamen Yaji Software Co., Ltd. reserves all rights not expressly granted to you.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/
#pragma once
#ifdef SWIGCOCOS
    #define IMPL_EVENT_TARGET_WITH_PARENT(...)
    #define IMPL_EVENT_TARGET(...)
    #define DECLARE_TARGET_EVENT_BEGIN(...)
    #define DECLARE_TARGET_EVENT_BEGIN_WITH_PARENTS(...)
    #define DECLARE_TARGET_EVENT_END(...)
    #define TARGET_EVENT_ARG0(...)
    #define TARGET_EVENT_ARG1(...)
    #define TARGET_EVENT_ARG2(...)
    #define TARGET_EVENT_ARG3(...)
    #define TARGET_EVENT_ARG4(...)
    #define TARGET_EVENT_ARG5(...)
    #define TARGET_EVENT_ARG6(...)
    #define TARGET_EVENT_ARG7(...)
    #define TARGET_EVENT_ARG8(...)
    #define TARGET_EVENT_ARG9(...)
    #define TARGET_EVENT_ARG10(...)
    #define TARGET_EVENT_ARG11(...)
    #define TARGET_EVENT_ARG12(...)
    #define TARGET_EVENT_ARG13(...)
    #define TARGET_EVENT_ARG14(...)
    #define TARGET_EVENT_ARG15(...)
    #define TARGET_EVENT_ARG16(...)
    #define TARGET_EVENT_ARG17(...)
    #define TARGET_EVENT_ARG18(...)
    #define TARGET_EVENT_ARG19(...)
    #define TARGET_EVENT_ARG20(...)
#else
    #include "EventBus.h"
    #include "EventTarget.h"
#endif