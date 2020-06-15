# paint-gui

[![GitHub Workflow Status](https://github.com/potados99/paint-gui/workflows/C/C++%20CI/badge.svg)](https://github.com/potados99/paint-gui/actions?query=workflow%3A%22C%2FC%2B%2B+CI%22)
[![GitHub last commit](https://img.shields.io/github/last-commit/potados99/paint-gui)](https://github.com/potados99/paint-gui/commits)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/potados99/paint-gui)](https://github.com/potados99/paint-gui/releases/latest)
[![GitHub stars](https://img.shields.io/github/stars/potados99/paint-gui?style=shield)](https://github.com/potados99/paint-gui/stargazers)
[![GitHub issues](https://img.shields.io/github/issues/potados99/paint-gui)](https://github.com/potados99/paint-gui/issues)
![GitHub closed issues](https://img.shields.io/github/issues-closed/potados99/paint-gui)
[![GitHub license](https://img.shields.io/github/license/potados99/paint-gui)](https://github.com/potados99/paint-gui/blob/master/LICENSE)

> 인천대학교 컴퓨터공학부 임베디드시스템 기말 과제

## 개요

<img src="./documents/at-a-glance.png" width="400"></img>

`Odroid-C1`에서 구동되는 GUI 그림판입니다.

## 상세

다음 기능을 지원합니다.

### 그리기

#### 도형

- `LINE`: 두 점을 잇는 선.
- `RECTANGLE`: 두 점을 맞꼭지점으로 하는 네모.
- `OVAL`: 두 점을 맞꼭지점으로 하는 네모에 접하는 타원.
- `FREE DRAW`: 입력되는 점으로 이루어진 임의의 도형.

> `LINE`과 `RECTANGLE`은 rubber-band를 지원합니다.

#### 옵션

- `PEN`: 윤곽선만 있는 도형을 그립니다.
- `FILL`: `RECTANGLE`과 `OVAL`에 한하여, 속이 채워져 있는 도형을 그립니다.

#### 색상

8가지 색상을 지원합니다.

### 선택하기

캔버스 위에 그려진 객체를 선택하여 이동합니다. 여러 도형이 겹쳐져 있을 경우 가장 위에 있는 도형이 선택됩니다.

> `LINE`과 `FREE DRAW` 의 경우, 해당 도형을 포함하는 가장 작은 직사각형 영역에 대해 선택이 가능합니다.

### 지우기

캔버스 위에 그려진 객체를 지웁니다. 지우개가 닿은 도형은 캔버스에서 사라집니다.

여러 도형이 겹쳐있을 경우, 가장 위에 있는 도형이 지워집니다.

## 실행 환경

#### 하드웨어

- Odroid-C1
- C1 3.2inch TFT+Touchscreen Shield

#### 소프트웨어

- Ubuntu 18.04.2 LTS (GNU/Linux 3.10.107-11 armv7l)
- GCC 7.3.0

## 기타

이 프로그램은 2019년 6월 3일부터 2019년 6월 16일까지 임베디드시스템 강의 기말 과제를 수행하기 위하여 제작되었습니다.

## 라이센스

소스 코드에는 `GPLv3` 라이센스가 적용됩니다. 라이센스는 [이곳](/LICENSE)에서 확인하실 수 있습니다.
