---
layout: "post"
title: "关于安卓与低功耗蓝牙设备连接的简易入门"
date: "2017-02-18 10:43"
author: Author
catalog: true
tags:
  - Andriod
  - 嵌入式系统
---

## 前言

写这一篇的动机是我最近的那个 year 2 project 正在做一个蓝牙车，用的芯片就是一块低功耗蓝牙（Bluetooth Low Energy, 简称 Bluetooth LE 或者 BLE），最惨的就是我在中文区几乎找不到一个比较好懂的教程，于是就打算开个小坑，同时也是给 report 打个草稿（笑）。这里我会主要集中在 Adafruit Bluefruit LE SPI Friend (2633) 这个板子，不过大致的思路基本上是差不多的。相关的操作一部分是我通过扒 Adafruit Bluefruit LE app 的源码得到的，但是因为这个 app 体量比较重，而且代码稍微有点陈旧，所以另外一部分我是通过啃 google 的文档或者在网上乱逛找到的。我争取在这篇文章里说人话（笑）。

## 声明权限

Android 中蓝牙相关的权限有这两条：

```xml
<uses-permission android:name="android.permission.BLUETOOTH" />
<uses-permission android:name="android.permission.BLUETOOTH_ADMIN" />
```

如果你用的是 Android Studio，它会自动提示你需要的权限，在需要时添加即可。

## 获得蓝牙管理器

蓝牙管理器是 Android 提供的最顶层的蓝牙接口，通过他可以调用到蓝牙相关的各类 API。要获得蓝牙管理器，你需要在你的 Activity 里边调用下面的代码。当然，你也可以把你的 Activity 传递到其他函数里进行调用，比如说自己写一个 `BluetoothHelper` 来处理相关的问题。

```java
BluetoothManager btManager =
    (BluetoothManager)getSystemService(BLUETOOTH_SERVICE);
```

## 获得蓝牙适配器

蓝牙适配器基本上就等同于手机上蓝牙硬件的抽象表示，提供了各种硬件操作相关的接口，可以通过一下代码获得：

```java
BluetoothAdapter adapter = btManager.getAdapter();
```

值得注意的一点是，如果这个设备根本有没有蓝牙硬件，这里的返回值会是 `null`，那么你可能需要进行相应的处理。另外，手机上的蓝牙设备可能并没有打开，相关状态你可以通过 `adapter.isEnabled()` 来获得。如果适配器还没有打开，是无法进行搜索操作的。你可以通过以下代码请求用户打开蓝牙：

```java
Intent enableBtIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
startActivityForResult(enableBtIntent, Activity.RESULT_OK);
```

这时系统就会通过弹窗请求开启蓝牙，请求的结果你可以通过重载 `Activity` 下的相关方法来监听：

```java
@Override
protected void onActivityResult(int request, int result, Intent data) {
    if (request == result)
        // 成功打开
    else
        // 用户拒绝
}
```

## 搜索设备

获得了蓝牙适配器之后，我们就可以开始搜索设备了。在老版本的 Android 里，低功耗蓝牙是没有独立的接口的，所以你可以调用以下代码进行搜索。这里你需要提供一个回调类，当蓝牙设备被发现的时候，系统会调用相应的方法将相关的 `BluetoothDevice` 传递给你。

```java
adapter.startLeScan(LeScanCallback callback);
```

在新版本的 Android，你可以使用以下代码，这里的 `ScanCallback` 是一个封装更好的回调类，它会传递一个 `ScanResult`,本质上还是把 `BluetoothDevice` 传递给你。除此之外，`ScanResult`中还包括了一些更多的信息，比如设备的信号强度，连接时间，以及设备的一些`Service`信息，这些我们之后再讲。

```java
adapter.getBluetoothLeScanner().startScan(ScanCallback callback)
```

本质上来讲，这两种方法基本是等价的，只是在更复杂的项目里，后者在管理上会方便一些。`ScanCallback` 在传递数据时，会把相关的数据解析再封装好传递给你，而 `LeScanCallback` 则需要手动管理和分析。手动分析的内容，实在要碰的话，可以参考[这一篇][1]。

## 低功耗蓝牙结构

在我们开始搜索之后，我们就会陆续接收到一些外部蓝牙设备。但是如何搜索到自己需要的设备呢？最简单的方法自然是通过名字过滤，直接调用 `bluetoothDevice.getName()` 就可以。不过很多时候我们可能要进行一些更精确的过滤，这就需要知道一些 BLE 的结构才行。每个 BLE 设备会提供若干个 `Service`，这里的 `Service` 基本上相当于这个设备的功能，比如说常见的串行收发（UURT），或者蓝牙键鼠。每一个 `Service` 是通过一个 `UUID` 进行识别的，这里的 `UUID` 是一个唯一的标识，至于他的值是多少，一部分是已经规定好了的，另一部分可能有厂家自定义的，这个我也没什么研究，网上有相关的文档可查。另外，使用一些阅读 BLE 设备的 App 可能会很有帮助。这里我用的是官方给的 `UUID "6e400001-b5a3-f393-e0a9-e50e24dcca9e"`。

要通过 `Service` 来过滤设备，可以参考下面的代码（这里我用 `ScanCallback` 举例，如果你用的是 `LeScanCallback`，可能要考虑手动分析 `UUID`）：

```java
UUID UUID_SERVICE = UUID.fromString("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
for (ParcelUuid uuid : result.getScanRecord().getServiceUuids()) {
    if (uuid.getUuid().equals(UUID_SERVICE)) {
        // 相关操作
    }
}
```

这里的 `result` 就是传递进来的 `ScanResult`。

在每个 `Service` 下，还有各种数据，叫做 `Characteristic`。这基本上就是 BLE 通信的实际内容了，两个蓝牙设备通过 `Characteristic` 的读写进行数据的交换。每个 `Characteristic` 也是通过 `UUID` 进行识别。在后面我们会说道具体的读写操作。

## 建立连接

经过上一步的过滤，你应该已经找到了需要连接的设备。这一步我们就要进行连接了。假设这个设备叫做 `device`，调用下列方法可以进行连接：

```java
BluetoothGatt gatt =
    device.connectGatt(context, false, new MyBluetoothGattCallback());
```

这里的 `context` 可以是你的 `Activity`， 第二个参数是是否自动连接，第三个参数是一个自定义的 `BluetoothGattCallback`，用来监听各种通信的事件，我们会在后面说道，下面我们就简写为 `GattCallback`。

建立通信之后，它会返回一个 `BluetoothGatt` 的对象，这个对象就是这个通信通道的抽象，提供一些通信状况相关的 API。这并不意味着连接已经完全建立了，只能说设备正在尝试进行连接。通常而言，建立连接需要的时间非常短，但是在特殊情况下，比如目标设备已经被占用时，通信就未必能成功建立了。相关的状态都会通过 `GattCallback` 传递过来。下面我就列举以下在不同时候你可能需要进行的操作：

#### `onConnectionStateChange`

当连接状态改变的时候（包括连接成功和掉线），这个函数会被触发。参考下列代码：

```java
@Override
public void onConnectionStateChange(BluetoothGatt gatt,
        int status, int newState) {
    if (newState == BluetoothProfile.STATE_CONNECTED) {
        gatt.discoverServices();
        // 连线
    } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
        // 断线
    }
}
```

比较重要的是这里要开始搜索服务，不然即使连接成功了，通过 `gatt` 也获得不了任何 `Service`。

#### `onServicesDiscovered`

紧接着上一步开始搜索服务之后，很快这个方法就会被触发。这个时候我们需要注册对一些服务的监听。

```java
@Override
public void onServicesDiscovered(BluetoothGatt gatt, int status) {
    final UUID UUID_SERVICE =
            UUID.fromString("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
    final UUID UUID_RX =
            UUID.fromString("6e400003-b5a3-f393-e0a9-e50e24dcca9e");
    final UUID CHARACT_CONFIG =
            UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");
    service = gatt.getService(UUID_SERVICE);  // 获得 Service
    BluetoothGattCharacteristic characteristic =
            service.getCharacteristic(UUID_RX);  // 获得 Characteristic
    BluetoothGattDescriptor descriptor =
            characteristic.getDescriptor(CHARACT_CONFIG);  // 获得 Descriptor
    // 和远程设备注册监听
    descriptor.setValue(BluetoothGattDescriptor.ENABLE_NOTIFICATION_VALUE);
    gatt.writeDescriptor(descriptor);  // 发送操作
    gatt.setCharacteristicNotification(characteristic, true);  // 本地注册
}
```

这里的几个 `UUID` 还是参考官方文档。在这一步一方面要向远程设备注册监听，另一方面还要和系统注册。注意这一步是至关重要的，否则你的监听器接收不到目标设备传来数据的提示。也就是 `onCharacteristicChanged` 不会被触发。

## 接收信息

当你完成上一步的监听注册之后，`onCharacteristicChanged` 方法就开始可以接收到传来的信息了。下面这一段代码进行了简单的检测，确定是需要监听的 `Characteristic`，然后就可以进行数据的读取了。当然这里你可能会想要把数据转换成字符串，可以考虑使用 `new String(bytes, Charset.fromString("UTF-8"))`.

```java
@Override
public void onCharacteristicChanged(BluetoothGatt gatt,
        BluetoothGattCharacteristic characteristic) {
    if (characteristic.getService().getUuid().equals(UUID_SERVICE)) {
        if (characteristic.getUuid().equals(UUID_RX)) {
            final byte[] bytes = characteristic.getValue();
            // 执行操作
        }
    }
}
```

## 发送信息

发送消息因为是主动的操作，所以就要简单一些，参考下面的代码：

```java
BluetoothGattCharacteristic characteristic =
        service.getCharacteristic(UUID_TX);
if (characteristic != null) {
    characteristic.setValue(chunk);
    gatt.writeCharacteristic(characteristic);
}
```

这个调用并不需要在特定的时候，唯一需要注意的是你在之前的步骤里要拿好相关对象的引用，这样才能调用到需要的方法。

## 断开连接

我非常建议你在结束时关闭连接。至少在我的设备上，如果没有完全关闭连接的话，下一次的连接会很吃力。参考下列代码：

```java
gatt.disconnect();
gatt.close();
```

我目前是把这一段代码放在 `Activity` 的 `onDestroy` 方法里，你也可以考虑在其他时候调用他来关闭。

## 结

如你所见，我这里只是把低功耗蓝牙相关的最核心的代码挑出来讲了一下，总代码量实际上是非常少的，一些细枝末节的东西我就没有细说了，比如说什么时候停止搜索蓝牙，如何确认连接的状态，不过我觉得到这个情况下你应该可以稍微摸索出这个系统的尿性了。如果你想看完整的代码，可以参考 [这里][2]，或者，如果你很有耐心的话，也可以参考 [这里][3]。

Have fun！

[1]: http://stackoverflow.com/questions/24003777/read-advertisement-packet-in-android
[2]: https://github.com/HelloRobotics/CarController/blob/master/app/src/main/java/io/github/hellorobotics/carcontroller/utils/HelperBle.java
[3]: https://github.com/adafruit/Adafruit_Android_BLE_UART
