# Overview
- Con trỏ là một biến lưu trữ địa chỉ của một đối tượng khác.Ví dụ nếu có một biến `int p` thì `p` là một biến lưu trữ giá trị kiểu `int`.Nếu có một con trỏ `int *q` thì `q` sẽ là con trỏ lưu địa chỉ của một biến kiểu `int`.
- Vậy khi có một hàm và hàm này thực hiện một số thao tác.Tất nhiên, khi tải chương trình này vào vi điều khiển, hàm này sẽ đượclưu trữ trong bộ nhớ mã của hàm này tức là mã được tạo ra để đại diện cho hàm này sẽ được lưu trong bộ nhớ mã (code memory), chính là bộ nhớ Flash.Điều đó có nghĩa là, hàm này sẽ có địa chỉ riêng của nó.
```C
void bar(void){
    printf("This is bar\n");
}
```
- Vậy địa chỉ hàm có thể biểu diễn trong chương trình bằng cách sử dụng tên của nó. Ta tự hỏi là có thể dùng một biến con trỏ lưu địa chỉ của hàm này không ???
```C
int *q;
q = &bar;
```
- Trong C khi ta làm như vậy thì sẽ gặp 1 cảnh báo "Assignment from incompatible pointer type" (Gán kiểu con trỏ không tương thích).Vì con trỏ bình thường được sử dụng để chứa địa chỉ trỏ tới dữ liệu.Đây lại là một địa chỉ trỏ đến mã (code), không phải dữ liệu.

- Vì vậy, trong C có thể tạo một biến con trỏ đặc biệt, mà có thể sử dụng để chứa địa chỉ của một hàm.Chính là con trỏ hàm.

# Syntax
- Khác với biến thông thường chỉ có đặc trưng là kiểu dữ liệu thì 1 hàm lại có các đặc trưng riêng là kiểu dữ liệu trả về và kiểu dữ liệu truyền vào hàm và số giá trị truyền vào hàm.
- Do vậy khi khai báo con trỏ hàm ta phải chỉ rõ kiểu dữ liệu trả về và các tham số của hàm. Ví dụ:

```C
void (*f)(void);
```
Ở đây, `f` là con trỏ hàm, có thể trỏ tới hàm không nhận tham số và không trả về giá trị.

- Để gán địa chỉ của một hàm cho con trỏ, bạn có thể sử dụng cú pháp:
```C
f = &bar;   // hoặc đơn giản là f = bar;
```

`&bar` là địa chỉ của hàm `bar` hoặc chỉ đơn giản là `bar` tuy nhiên việc sử dụng `&` là tốt hơn để rõ ràng rằng bạn đang lưu trữ địa chỉ hàm.

- Sau khi con trỏ hàm được gán địa chỉ của hàm, bạn có thể gọi hàm thông qua con trỏ hàm như sau:
```C
(*f)();  // Gọi hàm thông qua con trỏ f
// f();   Cách này có thể gây nhầm lẫn

```
# Con trỏ hàm với `typedef`
- Để dễ dàng quản lý và sử dụng con trỏ hàm nhiều lần, bạn có thể dùng `typedef` để tạo một kiểu mới cho con trỏ hàm.Ví dụ:
```C
int bar(int i){
    printf("This is bar: %d\n",i);
    return i;
}
typedef int(*f_t)(int);

int main()
{
    f_t f1,f2,f3;
    f1 = bar;
    f1(100);
    (*f1)(100); // cách này cũng được
}
```

# Cách truyền con trỏ hàm cho một hàm khác
- Giả sử có một biến `command`.Dựa trên giá trị của biến này,cần thực hiện một số hành động hoặc thực thi một số mã lệnh.Giả sử, nếu giá trị `command` là 0, bạn sẽ thực hiện phép cộng `a + b`.Nếu giá trị `command` là 1, bạn sẽ thực hiện phép trừ `a - b`.Nếu giá trị command là 2,sẽ thực hiện phép nhân `a * b`. Vậy thực hiện điều này như thế nào?

- Có thể sử dụng một câu lệnh `switch` trên biến này để thực hiện các hành động khác nhau hoặccó thể sử dụng một cấu trúc `if-else-if`.

- Vậy nếu không được sử dụng các câu lệnh so sánh như `switch` hoặc `if`, thì làm thế nào thực hiện được điều này? Dựa trên giá trị của `command`, cần thực hiện các hành động mà không sử dụng câu lệnh `if` hoặc `switch`.

Bài toán này có thể được giải quyết bằng cách sử dụng con trỏ hàm.
```C
#include <stdio.h>

// Định nghĩa kiểu con trỏ hàm
typedef int (*oper_t)(int, int);

// Các hàm thực hiện phép toán
int add(int a, int b) {
    return a + b;
}

int subtract(int a, int b) {
    return a - b;
}

int multiply(int a, int b) {
    return a * b;
}

// Hàm tổng quát nhận con trỏ hàm và thực thi
int operation(oper_t op, int a, int b) {
    return op(a, b);
}

int main() {
    // Khai báo và khởi tạo mảng con trỏ hàm
    oper_t operations[] = {add, subtract, multiply};
    
    int a = 10, b = 15;
    int command = 2; // Ví dụ với command = 2 (tức là phép nhân)

    // Gọi hàm tổng quát với con trỏ hàm từ mảng
    int result = operation(operations[command], a, b);
    
    printf("Result: %d\n", result); // In ra kết quả

    return 0;
}

```
