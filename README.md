# KdDeskTC 
是基于 rustdesk 开源项目结合自身业务需求二次开发的远程协助工具。


# KdDeskTC 项目概述

## MICROSOFT 基础类库：

应用程序向导已为您创建了此 KdDeskTC 应用程序。此应用程序不仅介绍了使用 Microsoft 基础类的基本知识，而且是编写应用程序的起点。

此文件包含组成 KdDeskTC 应用程序的每个文件的内容摘要。

KdDeskTC.vcproj

    这是使用“应用程序向导”生成的 VC++ 项目的主项目文件。 它包含有关生成文件的 Visual C++ 版本的信息，
    以及有关用“应用程序向导”所选择的平台、配置和项目功能的信息。

KdDeskTC.h

    这是应用程序的主头文件。  它包含其他项目特定的头文件（包括 Resource.h），并声明CKdDeskTCApp 应用程序类。

KdDeskTC.cpp

    这是包含应用程序类 CKdDeskTCApp 的主应用程序源文件。

KdDeskTC.rc

    这是程序使用的所有 Microsoft Windows 资源的列表。  它包含存储在 RES 子目录中的图标、位图和光标。  
    可直接在 Microsoft Visual C++ 中编辑此文件。 项目资源包含在 2052 中。

res\KdDeskTC.ico

    这是一个图标文件，用作应用程序的图标。  此图标包含在主资源文件 KdDeskTC.rc 中。

res\KdDeskTC.rc2

    此文件包含不由 Microsoft Visual C++ 编辑的资源。 应将所有不能由资源编辑器编辑的资源放在此文件中。


## 应用程序向导将创建一个对话框类：

KdDeskTCDlg.h、KdDeskTCDlg.cpp - 对话框

这些文件包含 CKdDeskTCDlg 类， 此类定义应用程序主对话框的行为。  
此对话框的模板包含在 KdDeskTC.rc 中，而此文件可以在 Microsoft Visual C++ 中进行编辑。


## 其他功能：

ActiveX 控件

    应用程序支持使用 ActiveX 控件。

## 其他标准文件：

StdAfx.h、StdAfx.cpp

    这些文件用于生成名为 KdDeskTC.pch 的预编译头文件 (PCH)和名为 StdAfx.obj 的预编译类型文件。

Resource.h

    这是标准头文件，它定义新资源 ID。Microsoft Visual C++ 将读取并更新此文件。

## 其他说明：

应用程序向导使用“TODO:” 来指示应添加或自定义的源代码部分。

如果应用程序在共享 DLL 中使用 MFC，且应用程序使用的语言不是操作系统的当前语言，则需要从 Microsoft Visual C++ 光盘上 
Win\System 目录下将相应的本地化资源 MFC70XXX.DLL 复制到计算机的 system 或 system32 目录下，
并将其重命名为 MFCLOC.DLL。  （“XXX”代表语言缩写。  例如，MFC70DEU.DLL 包含翻译成德语的资源。）  如果不这样做，应用程序的某些 UI 元素将保留为操作系统的语言。


