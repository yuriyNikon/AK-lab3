
/*
 * Copyright (c) 2017, GlobalLogic Ukraine LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the GlobalLogic.
 * 4. Neither the name of the GlobalLogic nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GLOBALLOGIC UKRAINE LLC ``AS IS`` AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL GLOBALLOGIC UKRAINE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/ktime.h>

MODULE_AUTHOR("Nykoniuk Yuriy <yurasik2810@gmail.com>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");

// Оголосити структуру для елемента списку
struct hello_entry {
    struct list_head list;
    ktime_t time;
};

// Створити статичну змінну голови списку
static LIST_HEAD(hello_list);

// Оголосити параметр
static unsigned int print_count = 1;
module_param(print_count, uint, S_IRUGO);
MODULE_PARM_DESC(print_count, "Number of times to print 'Hello, world!' (default=1)");

static int __init hello_init(void)
{
    if (print_count == 0 || (print_count > 5 && print_count < 10)) {
        // Надрукувати попередження та продовжити роботу
        pr_warn("Print count is 0 or between 5 and 10. Defaulting to 1.\n");
    } else if (print_count > 10) {
        // Надрукувати повідомлення про помилку та повернути -EINVAL
        pr_err("Print count is greater than 10. Module cannot be loaded.\n");
        return -EINVAL;
    }

    // Виділити пам'ять та додати елементи до списку відповідно до значень print_count
    while (print_count > 0) {
        struct hello_entry *entry = kmalloc(sizeof(*entry), GFP_KERNEL);

        if (!entry) {
            pr_err("Failed to allocate memory for hello_entry\n");
            return -ENOMEM;
        }

        // Зберегти поточний час ядра
        entry->time = ktime_get();

        // Додати елемент до списку
        list_add(&entry->list, &hello_list);

        // Надрукувати привітання
        pr_emerg("Hello, world!\n");

        print_count--;
    }

    return 0;
}

static void __exit hello_exit(void)
{
    struct hello_entry *entry, *temp;

    // Пройти по списку
    list_for_each_entry_safe(entry, temp, &hello_list, list) {
        // Надрукувати час події в наносекундах
        pr_emerg("Time: %lld ns\n", ktime_to_ns(entry->time));

        // Вилучити елемент зі списку
        list_del(&entry->list);

        // Звільнити виділену пам'ять
        kfree(entry);
    }
}

module_init(hello_init);
module_exit(hello_exit);
