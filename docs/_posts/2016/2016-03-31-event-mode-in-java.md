---
layout: post
title:  "关于java事件处理模式的纠结日常"
date:   2016-03-31
author: Author
catalog:    true
tags: [java]
---

## 写在前边

> 想到这个空间还算是挺私人的，所以以后可能写东西之前就会更一点无聊的日常，不然搞得一本正经的有点怪怪的（笑）。这次要说的呢是我目前手上mod的开发问题。目前的开发呢已经逼近尾声了，在仅有的几个计划功能开发完之后，大概会补一个1.7的版本，如果我真的能搞定1.7的材质以及nei的api的话（雾）。这个就是我最近失踪的原因啦，而且因为又开学了有可能还会失踪一段时间。就是这样啦。

不出意外，上边这段文字是我一个月之前刚开学的时候写的，现在已经逼近期中考试了。平时觉得牛逼的程序员都有点拖延症，所以就这样吧，不要在意。另外呢，现在mod的开发已经完成多时，1.7 的版本也已经更新多版了，1.9 尚在计划中。详情戳 [这里][1]，我就不多说了。另外呢，我正在考虑限制文章的篇幅，800 字以内？嗯，就是这样。

## 初探

我们先来说一些铺垫的东西，何为事件：事件就是用户与程序界面的交互行为。鼠标点击，移动，键盘敲击，都可以算做事件。所以，最简单的程序，就算我们平时写命令行窗口也会有相关的事件，不过这种情况下处理都比较简单，大部分内容系统已经搞定了，我们只需要用类似于 `stdin` （C） 或者 `System.in` 这样的东西直接获取结果就好了。我们这里主要说 GUI 相关的事件处理，比如系统是怎样执行每一个按钮的点击，执行文本框的输入的。Java 中最广泛使用的是所谓 `ActionListener` 的系统，不过这里也会讲到一些其他场景的模型。

![GUI 树形结构](/img/posts/event-mode-in-java_1.jpg)

对于大部分的界面而言，都会有一个树形的结构。在一个GUI中，我们有一个程序，持有多个窗口，每个窗口里边有多个面板，每个面板装着多个元件。这个模型有点像历史书上的封建制度“我附庸的附庸不是我的附庸”。在这个模型下，我们也可以这样说：“我持有的引用所持有的引用不是我持有的引用”。这话说着有点拗口，意思就是，当一个程序收到了点击的信号，他不可能直接找到那个被点击的元件然后直接激活。因为这个程序只持有若干个窗口的引用，而不直接持有被点击的元件。在这种情况下，所谓的“责任者模式”也就呼之欲出了。这里我必须要声明一下，我们这里的“树形”只是人为规定的结构，而不是一种硬性的规定。一个窗口直接持有元件的引用也很常见，但是在更宽泛的情况下，树形结构更易于项目的管理。

## 责任者模式

这是一个被广泛使用的事件模型，上层组件收到时间信息的时候，就将事件内容转交给下一层对象，下一层对象处理完事件后，将结果返回给上层对象。如果还不是很确切的话，看代码好了：

```java
abstract class Component{
    boolean onClick(int mouseX, int mouseY);
}

class Panel extends Component{
    public list<Component> components;  // 存储所有下层对象的应用

    @Override
    boolean onClick(int mouseX, int mouseY){    // 处理点击事件
        for(Component comp : components){   // 逐个调用下层对象进行事件处理
            if(comp.onClick(mouseX, mouseY)){
                return true;    // 如果事件被处理，返回结果
            }
        }
        return false;   // 如果事件未被处理，返回结果
    }
}
```

这一段代码只是一个抽象，只举了鼠标点击事件的传递，实际情况下，实现可能更为复杂，但是原理是一致的。

## 监听者模式

上边我们已经说了上层对象触发下层对象的情况。我们再来说下层对象的事件触发上层对象动作的情况。有人可能要问了：下层对象为什么会触发上层对象呢？答案很明显，每一次鼠标点击的事件都会经由上级对象转交给下级对象，但是这就结束了么？答案显然是否定的。按钮被触发之后，这一结果还会传递给其它组件，进行相应的反馈。

当然这种模式也有很多实现方式，先说最简单的，下层对象直接触发上层对象处理。先来看代码：

```java
abstract class Component{
    boolean onClick(int mouseX, int mouseY);
}

class Panel extends Component{
    public List<Component> components = new ArrayList<>();

    Panel(){
        components.add(new Button(this));
    }

    void actionPerformed(){
        System.out.println("事件被处理啦！");
    }

    // 顺次触发所有组件并反馈结果
    @Override
    boolean onClick(int mouseX, int mouseY){
        for(Component comp : components){
            if(comp.onClick(mouseX, mouseY)){
                return true;
            }
        }
        return false;
    }
}

class Button extends Component{
    Panel parent;   // 存放上级对象

    Button(Panel p){
        parent = p;
    }

    onClick(int mouseX, int mouseY){  // 处理点击事件
        parent.actionPerformed();   // 调用上级的处理函数
    }
}
```

感觉代码已经难以避免的变长了（笑）。从代码的长度也可以比较明白的显示出结构的复杂程度。在这种情况下，下层对象直接持有上层引用，一旦接收到事件，就可以直接调用。但是这样弊端也很明显，这种触发形式是内建在代码里的，所以拓展性很不好。于是我们将子类接收到事件时需要进行的动作装在一个列表里，并且允许其他类来增减，这就是普遍使用的监听者模型。插一段代码吧：

```java
abstract class Component{
    boolean onClick(int mouseX, int mouseY);
}

interface ActionListener{
    void actionPerformed();
}

class Panel extends Component{
    public List<Component> components = new ArrayList<>();

    // 构造函数中添加按钮
    Panel(){
        components.add(new Button(this)){
            {
                // 给按钮添加监听器
                actionListeners.add(new ActionListener(){
                    @Override
                    void actionPerformed(){
                        System.out.println("发现事件！");
                    }
                });
            }
        };
    }

    // 顺次触发所有组件并反馈结果
    @Override
    boolean onClick(int mouseX, int mouseY){
        for(Component comp : components){
            if(comp.onClick(mouseX, mouseY)){
                return true;
            }
        }
        return false;
    }
}

class Button extends Component{
    public List<ActionListener> actionListeners;

    @Override
    onClick(int mouseX, int mouseY){  // 处理点击事件
        for(ActionListener listener : actionListeners){
            listener.actionPerformed();
        }
    }
}
```

在这种模式里，监听类多数为上层类的内部类（普通内部类，匿名内部类，lamda表达式的匿名内部类均在此列，不细讲了），上层类将监听类塞入下层对象的监听列表中，下层对象每当接收到输入信号时，就把这一信号传递给监听列表中的所有监听器，经由监听器传递给上层对象。比起上一种实现方式，监听器模型显然份量更重，不过它也实现了更好的拓展性:我们可以自由改动组件被触发时的行为。这里我们就看出程序设计的基本原理：效率与拓展性不可得兼。

## 事件总线模型

如果你还没有被我繁琐的语言逼疯的话，那么恭喜你，我们重点的内容就要开始了。上边我们一直在说树形结构以内的事件传递，这其实是比较好处理的，因为获取引用还不至于是一件非常困难的事。但是，如果我们需要在事件的发生和执行中构造一种松耦合的关系，问题就有些复杂了。比如说，我有一个数据库，里边有很多用户，我需要监听用户的某些活动。这你可能会说，在用户对象构造的时候直接挂上监听器不就好了么？确实是这样，但是如果我要开发的是一个插件，并不能修改原有的构造代码，我应该怎么做呢？我们就必须引入事件总线模型了，在程序设计模式中，比较相似的是命令模式。

在这一结构中，系统维护着一套事件总线，一旦有事件发生，对象都将发生的事打包发布到事件总线，再由事件总线转交给登记的订阅者。这一模式最大的优势在于他是基于类的，订阅者只需要告知总线自己订阅的事件类型，而不必要获得事件的发布者。在总线转交事件的过程中，发布者的引用也被一并转交，供监听者进行处理，这样，拿不到引用的问题就被解决了。不过，我们的结构不得不被再一次扩大（笑）。不过好在我们这次的内容已经跟 `Panel` 类没什么关系了，所以我们的代码也许能少一些。

```java
abstract class Component{
    boolean onClick(int mouseX, int mouseY);
}

interface ActionListener{
    void actionPerformed(Event e);
}

class Event{
    Event(int x, int y, Component trig){
        mouseX = x;
        mouseY = y;
        trigger = trig;
    }

    int mouseX; // 鼠标信息
    int mouseY;
    Component trigger;  // 触发者引用
}

static class EventBus{
    public static List<ActionListener> actionListeners;

    static void postEvent(Event e){
        for(ActionListener listener : actionListeners){
            listener.actionPerformed(e);
        }
    }
}

class Button extends Component{
    @Override
    onClick(int mouseX, int mouseY){  // 处理点击事件
        EventBus.postEvent(new Event(mouseX, mouseY, this));  // 发布事件
    }
}
```

这段代码看起来似乎不长，但是执行起来是比上边的模式效率低一些的。设想一下，在这个数据库里，如果用户有大量的事件触发，同时挂载的订阅者又很多，整个事件总线就会疲劳不堪。因为事件总线模式是通过静态的事件总线类来中转，每当有事件触发，他就要遍历所有的订阅者来发送事件，而每个订阅者都要处理一通，这样的效率是不高的，而且在这些事件中，有相当一部分对于监听者是没用的，会被过滤掉，在这个过滤过程中，又会损失一部分性能。但是在这种复杂情况下，事件总线几乎是仅有的选择了，这就能看出事件总线的特点：关注某一类事件，而不关注某一个对象；良好的可扩展性。现在大部分程序都会提供插件开发的接口，这样的特性就尤为重要了。

我这里的总线只是给出的最小可运行体积，实际应用中可能会有相当复杂的结构出现，但是原理仍然是一致的。另外呢，想到我几个月之前在朋友圈说的那句话“总线=Bus=公交车”，这是因为这个事件是在所有订阅者中传阅的，每个订阅者都可以进行修改，这也就意味着等传给最后一个订阅者手中的时候，这个事件可能已经被篡改的面目全非了（污），甚至某些总线会在发布之后把事件返回给发布者，请自行脑补发布者的内心波动（此处应有表情：我内心毫无波动，甚至还想笑）。大概就是这样吧（笑）。

## 写在后边

看了这么多不同的模式，想必读者对于程序设计的思路也有一些理解了，最明显的特征还是那句话：性能与功能不可得兼。根据不同的需求选择需要的模式才是一个优秀程序员应该做的，盲目追求功能而牺牲性能并不是好的做法。还有，说好的限制篇幅呢！对于这种事情，我能说的只有“妈的智障”！

事件总线参考：[szszss的博客：基于FML的MinecraftMod制作教程(1)-Forge的事件系统][2]

[1]: http://minecraft.curseforge.com/projects/just-enough-calculation
[2]: http://blog.hakugyokurou.net/?p=225
