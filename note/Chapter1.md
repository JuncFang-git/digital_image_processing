# 1.2 图像的颜色空间

## 1.2.1RGB颜色空间

RGB颜色空间由红(Red)，绿(Green)，蓝(Blue)三个分量组成。三个分量相互独立，取值范围都为[0, 255]，共256个值。RGB模式在笛卡尔坐标系中表现为一个颜色立方体，总共可以表示的颜色数量为$256^3$。

## 1.2.2 HSV颜色空间

HSV颜色空间由色相(Hue)，饱和度(Saturation)，明度(Value)，三个分量组成。具体来说

- 色相（Hue）：用以表示色彩，用一个[0&deg;, 360&deg;]的圆形表示不同色彩。其中0&deg;表示红色，60&deg;表示黄色，120&deg;表示绿色，180&deg;表示青色，240&deg;表示蓝色，300&deg;表示品红色，以此与rgb的立方体顶点对应起来。具体可见下图：
  ![hue](https://img-blog.csdnimg.cn/20200522090334587.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzQyOTYyNjIy,size_16,color_FFFFFF,t_70)
- 饱和度（Saturation）：用于表示色彩的纯度，一种颜色的饱和度越高，它就越鲜艳；反之，一种颜色的饱和度越低，它就越接近于灰色调。饱和度的取值范围为[0, 1.0]。
- 明度（Value）：用于表示色彩的明亮程度，值越高色彩越亮，值越低色彩越暗。取值范围为[0, 1.0]。

HSV的颜色空间模型是一个棱锥，如下图所示。由图可见，当$V=0$时，HSV只能表示黑色。当$S=0,V\neq0$时，HSV只能表示不同亮度的灰色。

<div align=center><img src="https://img2018.cnblogs.com/blog/1528422/201909/1528422-20190910193823543-563385632.png"></div>

HSV与RGB转换公式如下：

- RGB转HSV

$r=R/255, g=G/255, b=B/255 $

$max = max(r,g,b); \ \ \ \    min = min(rgb);$

$$
H = \begin{cases}
0, & min = max\\
60\times\frac{g - b}{max - min} + 0, & max=r, \ g\geq b \\
60\times\frac{g-b}{max-min} + 360, & max=r, \ g < b \\
60\times\frac{b - r}{max - min} + 120, & max = g \\
60\times\frac{r - g}{max - min} + 240, & max = b \hspace{50cm}
\end{cases}
$$

$$
S = \begin{cases} 
0, & max = 0 \\ 
\frac{max - min}{max}, &others  \hspace{50cm}
\end{cases}
$$

$V = max$

- HSV转RGB

$h = \lfloor\frac{H}{60}\rfloor$

$f = \frac{H}{60} - h$

$p = V\times(1 - S)$

$q = V\times(1 - f\times S)$

$t = V\times(1 - (1- f)\times S)$

$$
(R,G,B) = \begin{cases}
(V, t, p), & h=0\\
(q, V, p), & h=1 \\
(p, V, t), & h = 2 \\
(p, q, V), & h = 3 \\
(t, p, V), & h = 4 \\
(V, p, q) & h = 5 \hspace{50cm}
\end{cases}
$$

## 1.2.3 YUV颜色空间

YUV多用于电视视频标准中，黑白电视系统仅使用亮度Y，彩色电视系统中则会对U，V惊醒解码从而产生彩色画面。目前Camera实时处理多采用YUV信号流，需要了解YUV和RGB的转换关系才能处理对应场景。YUV分量中，Y表示明亮度，取值为[0, 255]；U表示色度，取值为[-122, 122]；V表示浓度，取值为[-157, 157]。
基于Gamma校准的RGB与YUV颜色空间转换公式如下：

- RGB转YUV
  $Y = 0.299R + 0.587G + 0.114B$
  $U = -0.147R - 0.289G + 0.436B$
  $V = 0.615R  -  0.515G - 0.100B$
  
  -YUV转RGB
  
  $R = Y + 1.140V$
  $G = Y - 0.395U - 0.581V$
  $B = Y + 2.032U$

