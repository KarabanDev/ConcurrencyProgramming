//
//  ViewController.h
//  ExampleOfConcurrency
//
//  Created by Andrey Karaban on 21.12.15.
//  Copyright © 2015 Andrey Karaban. All rights reserved.
//

#import <UIKit/UIKit.h>
@interface EOCMainViewController : UIViewController{
    
}
/*
 -------------------#pragma mark - DISPATCH QUEUES with GCD----------------------
 
 Мультипоточность/ параллельное выполнение - для чего это нужно:
    1. Используйте аппаратное обеспечение вашего дивайса: Современные дивайсы имееют несколько ядер, что позволяет выполнять несколько задач параллельно. Поэтому необходимо использовать мультипоточность, чтобы получить максимум от железа.
 
    2. Улучшение в плане user expirience: Все ресурсоемкие и тяжелые задачи (например, загрузка с web-сервера) должны проходить в background-е и не затрагивать главный поток выполнения(main thread), в котором происходит обработка UI-эелементов.
 
 Эти две основные задачи легко решаемы при помощи предоставленных API - NSOperation и dispatch queues (очереди отправки)!!!
 
 Part 1: GCD (Grand Central Dispatch) - часть первая: GCD - это наиболее часто используемое API для управления параллельным кодом и выполнения задач асинхронно на Unix-уровне вашей системы. GCD преодоставляет и выпоняет очереди задач. 
 Для начала разберемся, что же означают очереди
    Очереди - это структуры данных, которые управляют объектами в порядке First-in, First-out (FIFO) - это означает, что объект (или некие данные) первым добавленный в очередь будет и первым обработанным и из очереди исключенным.
 
 
 Dispatch Queues - отправка очередей
 Это самый простой способ выполнять задачи асинхронно или параллельно в вашем приложении. Очередь - это когда задачи представляются вашим приложением в форме блоков (блоков кода). Есть 2 варианта отправки очереди (1)последовательная отправка (2)параллельная отправка.
**** - Необходимо понимать, что задачи присвоенные обоим очередям, будут выполнятся в разных потоках, отличных от того, где они были созданы.Другими словами, вы создаете блоки кода и передаете на отправку очередей в основном потоке. Но все эти задачи (блоки кода) будут происходить в другом потоке.
 
 
 Serial Queues - последовательные очереди - в такой очереди одновременно может выполнятся только одна задача.Каждая последующая задача будет ожидать завершения предыдущей. Но эта очередь абсолятно не беспокоится, что происходит в другой - это означает, что также можно выполнять задачи параллельно, создав другую последовательную очередь.
 Преимущества последовательных очередей:
 1.Последовательные очереди идеально подходят для управления общими ресурсами. Они гарантируют сереализованный доступ к общему ресурсу и предотвращают состояние гонки.
 2.Задачи выполнятся в строгом порядке.
 3.Можно создавать любое количество последовательных очередей
 
 
 Concurrent Queues - параллельные очереди - как следует из названия, позволяют одновременно выполнять много задач.Задачи выполняются в порядке добавления в очередь. Но для их запуска им не надо ждать одной другую. Параллельные очереди гарантируют, что выполнятся задачи будут в том же порядке, но вы не будете знать порядок выполнения, время выполнения и количества выполняемых задач в данный момент. 
 
 
 По умолчанию, система обеспечивает каждое приложение 5-ю очередями - одной последовательной(исполнение которой происходит в основном потоке - она отвечает за обновление UI  и выполняет все задачи связанные с UIView) и 4-мя параллельными - Global Dispatch queues
 
 В ходе выполнения видно различия в скорости загрузки и появления изображений в приложении.
 
 -------------------#pragma mark - Operation Queues----------------------
 
 GCD - это низкоуровневый С API, позволяющий осуществлять многозадачность. Очереди операций - это высокоуровневая абстракция, построенная на основе GCD, представленная в Объектно-Ориентированном виде.
 Основные различия между очередями оперций и отправкой очередей
 1. операции не отвечают протоколу FIFO - мы можем определить оередь выполнения только задав некую зависимость одной операции от другой
 2. по дефолту операции - параллельны- вы не можете задать им последовательное выполнение.
 3.Очереди операций - это экземпляры класса NSOperationQueue а их задачи (блоки кода) инкапсулируются в экземпляры класса NSOperation
 
 Задачи передаваемые на исполнение в очередь операций представлены в виде экземпляров класса NSOperation.
 NSOperation - это абстрактный класс, поэтому вы не можете напрямую его использовать . В нашем SDK представлены 2 конкретных подкласса :
 
 1.NSBlockOperation - используйте этоткласс  для инициализации операций с блоками.Операция может содержать не один блок -и будет считаться завершенной только после выполнения всех блоков
 2. NSINvocationOperation -Используйте этот класс, чтобы инициировать операцию, которая состоит из вызова селектора на указанный объект.
 
 
 Преимущества NSOperation:
 1. Вы можете назначать зависимости - то есть след. операция не будет выполнена пока завершится предыдущая(не обязательно по порядку, как говорилось ранее Operations Queue не отвечают FIFO)
 2. Можно назначить приоритет выполнения - через свойство queuePriority
 
 public enum NSOperationQueuePriority : Int {
 case VeryLow
 case Low
 case Normal
 case High
 case VeryHigh
 }
 
 3.Вы можете остановить выполнение операции или всех опреаций для заданной очереди. Операция может быть отмененан после добавления в очередь, посредством вызова метода cancel() класса NSOperation. 
 После отмены операции есть 3 возможных сценария развития событий:
    3.1.Если она уже завершена - ничего не произойдет
    3.2.Если она в процессе выполнения система не завершит ее принудительно, выполнение продолжится а свойство cancelled у операции примет значение TRUE
    3.3. Если она добавлена в очередь и только ЖДЕТ выполнения - тогда она удалится из очереди и не будет выполнена.
 4.У оперций есть 3 важных свойства:
    4.1 - Finished - принимает значение TRUE после полного выполнения
    4.2 - Cancelled - принимает значение TRUE после отмены 
    4.3 - READY - принимает значение TRUE перед непосредственным выполнением.
 5. Есть возможность установить блок завершения, который вызовется когда свойство finished -> true
 */


@end

