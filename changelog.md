# LTE-Mリーフ 更新履歴

## 更新内容  

### STM32 core with LPWA 1.9.1-Relase2 (2021/2/1)
TCP,UDP,HTTPライブラリにて接続先に長いURLを設定するとボードがハングアップする事象に対応  
ボード内割り込みリソース定義を追加
```
#define SENS_PIR_IRQN PE0 // PIR sensor interrupt (active Low)
#define SENS_OPT_IRQN PE1 // Optical sensor interrupt (active Low)
#define SENS_6DOF_IRQ PE2 // 6axis sensor interrupt (active High)
```

### STM32 core with LPWA 1.9.1-Relase1 (2021/1/15)
初回リリース  
