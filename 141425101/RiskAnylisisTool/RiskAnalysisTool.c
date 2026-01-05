#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

// Максимум стоки и дни(минали, симулирани, бъдещи)
#define MAX_STOCKS 5
#define PAST_DAYS 14
#define SIMULATION_DAYS 7
#define FUTURE_DAYS 14

// Структура за стока
typedef struct {
    char name[20];
    double past_prices[PAST_DAYS];  // Последни 14 дни
    double game_prices[SIMULATION_DAYS + 1];  // Цени за всеки симулиран ден
    double current_price;
    double daily_change[PAST_DAYS-1];  // Дневна промяна в процент %
    double avg_daily_change;          // Средна промяна в процент %
    double volatility;                // How bouncy it is
} Stock;

// Структура на инвестиция в симулиран ден
typedef struct {
    int stock_index;      // Коя стока (0-4)
    double amount;        // Колко пари се инвестират
    int day_invested;     // Кой ден от симулацията (0-6)
} Investment;



// Функции
void generate_random_stocks(Stock stocks[], int count);
void print_stock_history(Stock stocks[], int count);
void print_daily_decision(int day, double cash);
double simulate_day(Stock stocks[], int count, int day);
void print_results(Stock stocks[], Investment investments[], int inv_count, double starting_cash);
void print_future_prediction(Stock stocks[], int count);

int main() {
    int num_stocks;
    double cash = 1000.0;  // Начална сума
    Investment investments[50];  // Място за всички инвестиции
    int investment_count = 0;
    
    srand(time(NULL));
    
    printf("===========================================\n");
    printf("      7-ДНИ ИНВЕСТИЦИОНЕН СИМУЛАТОР.       \n");
    printf("===========================================\n\n");
    
    // Част 1: Пита колко стоки да има
    printf("Имаш $%.2f, които да инвестираш за 7 дни.\n\n", cash);
    
    do {
        printf("В колко стоки искаш да инвестираш? (1-5): ");
        scanf("%d", &num_stocks);
        
        if(num_stocks < 1 || num_stocks > 5) {
            printf("Моля въведете число между 1 и 5.\n");
        }
    } while(num_stocks < 1 || num_stocks > 5);
    
    // Създаване на стоки
    Stock stocks[MAX_STOCKS];
    generate_random_stocks(stocks, num_stocks);
    
    // Част 2: Показва информация за последните 14 дни на всяка стока
    printf("\n===========================================\n");
    printf("               ПОСЛЕДНИ 14 ДНИ             \n");
    printf("===========================================\n\n");
    print_stock_history(stocks, num_stocks);
    
    // Част 3: 7-дневна симулация
    printf("\n===========================================\n");
    printf("              7-ДНЕВНА СИМУЛАЦИЯ           \n");
    printf("===========================================\n\n");
    
    for(int day = 0; day < SIMULATION_DAYS; day++) {
        print_daily_decision(day + 1, cash);
        
        // Показва сегашни цени на стоките
        printf("\nСегашни стокови цени:\n");
        printf("---------------------\n");
        for(int i = 0; i < num_stocks; i++) {
            printf("%d. %-15s: $%.2f (Днес: %+.2f%%)\n", 
                   i + 1, 
                   stocks[i].name, 
                   stocks[i].current_price,
                   stocks[i].daily_change[PAST_DAYS - 2]);  // Вчеращна промяна
        }
        
        // Пита за инвестиционно решение
        int choice;
        double amount;
        
        printf("\nТвоето инвестиционно решение:\n");
        printf("В коя стока искаш да инвестираш? (1-%d, 0 за пропускане): ", num_stocks);
        scanf("%d", &choice);
        
        if(choice > 0 && choice <= num_stocks) {
            printf("Каква сума искаш да инвестираш в %s? (Имаш $%.2f): $", 
                   stocks[choice-1].name, cash);
            scanf("%lf", &amount);
            
            if(amount > cash) {
                printf("Нямаш толкова пари! Пропускаме днес.\n");
            } else if(amount > 0) {
                // Записва инвестиция
                investments[investment_count].stock_index = choice - 1;
                investments[investment_count].amount = amount;
                investments[investment_count].day_invested = day;
                investment_count++;
                
                cash -= amount;
                printf("Инвестирахте $%.2f в %s. Оставащи пари: $%.2f\n", 
                       amount, stocks[choice-1].name, cash);
            }
        }
        
        // Симулира промяна на маркета за деня
        double market_change = simulate_day(stocks, num_stocks, day);
        printf("\nМаркета се промени с: %+.2f%% като цяло днес.\n", market_change);
        printf("Натиснете Enter за да отидете на следващия ден...");
        getchar();
        getchar();  // Изчиства място в терминала
        printf("\n");
    }
    
    // Част 4: Показва финалните резултати
    printf("\n===========================================\n");
    printf("            ФИНАЛНИ РЕЗУЛТАТИ              \n");
    printf("===========================================\n\n");
    print_results(stocks, investments, investment_count, 1000.0);
    
    // Step 5: Show future prediction
    printf("\n===========================================\n");
    printf("        14-ДНЕВНА БЪДЕЩА ПРЕДСКАЗКА        \n");
    printf("===========================================\n\n");
    print_future_prediction(stocks, num_stocks);
    
    return 0;
}

// Генерира случайни стойности на стоките
void generate_random_stocks(Stock stocks[], int count) {
    char *stock_names[] = {"Apple", "Microsoft", "Google", 
                          "Netflix", "Spotify"};
    
    for(int i = 0; i < count; i++) {
        // Слага име на стока
        strcpy(stocks[i].name, stock_names[i]);
        
        // Генерира случайна начална цена на стоката между $50 и $200
        stocks[i].past_prices[0] = 50 + (rand() % 151);
        
        // Генерира 14 дни цени със случайни промени
        for(int day = 1; day < PAST_DAYS; day++) {
            double change_percent = ((rand() % 201) - 100) / 1000.0;  // от -10% до +10%
            stocks[i].past_prices[day] = stocks[i].past_prices[day-1] * 
                                        (1.0 + change_percent);
            
            // Калкулира дневната промяна
            stocks[i].daily_change[day-1] = change_percent * 100;  // Запазва като процент %
        }
        
        // Калкулира средната дневна промяна
        double sum = 0;
        for(int day = 0; day < PAST_DAYS-1; day++) {
            sum += stocks[i].daily_change[day];
        }
        stocks[i].avg_daily_change = sum / (PAST_DAYS - 1);

        // Запазва сегашната цена като цената на последния ден
        stocks[i].current_price = stocks[i].past_prices[PAST_DAYS - 1];
        
        // Калкулира волиталността
        double variance = 0;
        for(int day = 0; day < PAST_DAYS-1; day++) {
            double diff = stocks[i].daily_change[day] - stocks[i].avg_daily_change;
            variance += diff * diff; // Правим разликата позитивно число
        }
        stocks[i].volatility = sqrt(variance / (PAST_DAYS - 1));
    }
}

// Изкарва историята на стоките
void print_stock_history(Stock stocks[], int count) {
    for(int i = 0; i < count; i++) {
        printf("%d. %s\n", i + 1, stocks[i].name);
        printf("   Сегашна цена: $%.2f\n", stocks[i].current_price);
        printf("   Средна дневна промяна: %+.2f%%\n", stocks[i].avg_daily_change);
        printf("   Волиталност: %.2f%% (По-високо = По-рисково)\n", stocks[i].volatility);
        printf("   Последни 5 дни: ");
        
        for(int day = PAST_DAYS-5; day < PAST_DAYS; day++) {
            printf("$%.1f ", stocks[i].past_prices[day]);
        }
        printf("\n\n");
    }
}

// Изкарва информация за кой ден от симулацията е и останали пари
void print_daily_decision(int day, double cash) {
    printf("DAY %d/7\n", day);
    printf("--------\n");
    printf("Available Cash: $%.2f\n\n", cash);
}

// Симулира един ден от промени в маркета
double simulate_day(Stock stocks[], int count, int day) {
    double overall_market_change = ((rand() % 75) - 25) / 1000.0;  // от -2.5% до +5%

    for(int i = 0; i < count; i++) {
        // Стокова промяна = обща промяна в маркета + индивидуална промяна в стоките
        double stock_change = (rand() % 76 - 25) / 1000.0; // от -2.5% до +7.5%
        
        // Обновява сегашната цена
        stocks[i].current_price *= (1.0 + stock_change);

        // Обновяваме цените на Симулационните дни
        stocks[i].game_prices[day] = stocks[i].current_price;
        
        // Обновява дневната промяна на последните 14 дни
        for(int j = 0;j < PAST_DAYS - 1;j++)
        {
            stocks[i].daily_change[j] = stocks[i].daily_change[j + 1]; // Местим индекса на всеки ден назад с 1
        }
        stocks[i].daily_change[PAST_DAYS - 2] = stock_change * 100;
    }
    
    return overall_market_change * 100;  // Връща общата промяна в маркета като процент
}

// Изкарва финалните резултати
void print_results(Stock stocks[], Investment investments[], int inv_count, double starting_cash) {
    double total_value = 0;
    double cash_remaining = starting_cash;
    
    printf("Твоите инвестиции:\n");
    printf("-----------------\n");
    
    for(int i = 0; i < inv_count; i++) {
        Investment inv = investments[i];
        double current_value = inv.amount * 
                              (stocks[inv.stock_index].current_price / 
                               stocks[inv.stock_index].past_prices[(PAST_DAYS) - 7 + inv.day_invested]);
        
        double profit_loss = current_value - inv.amount;
        double percent_change = (profit_loss / inv.amount) * 100;
        
        printf("%-15s: Инвестирани $%.2f → Сега $%.2f (%+.2f%%)\n", 
               stocks[inv.stock_index].name, 
               inv.amount, 
               current_value, 
               percent_change);
        
        total_value += current_value;
        cash_remaining -= inv.amount;
    }
    
    double final_total = total_value + cash_remaining;
    double total_profit = final_total - starting_cash;
    double total_percent = (total_profit / starting_cash) * 100;
    
    printf("\nОбобщение:\n");
    printf("--------\n");
    printf("Начална сума: $%.2f\n", starting_cash);
    printf("Оставаща сума: $%.2f\n", cash_remaining);
    printf("Стойност на инвестициите: $%.2f\n", total_value);
    printf("Обща нет стойност: $%.2f\n", final_total);
    printf("Обща печалба/загуба: $%+.2f (%+.2f%%)\n", total_profit, total_percent);
    
    printf("\nОценка на представяне: ");
    if(total_percent > 10) printf("ПРЕКРАСНО! ★★★★★\n");
    else if(total_percent > 5) printf("Супер! ★★★★\n");
    else if(total_percent > 0) printf("Не е зле! ★★★\n");
    else if(total_percent > -5) printf("Лека загуба ★★\n");
    else printf("Трябва подобрение ★\n");
}

// Изважда 14-дневна бъдеща предсказка
void print_future_prediction(Stock stocks[], int count) {
    printf("Ако продължите за още 14 дни (предсказка):\n\n");
    
    for(int i = 0; i < count; i++) {
        double predicted_price = stocks[i].current_price;
        double predicted_change_total = 0;
        
        // Симулира 14 дни в бъдещето използвайки средната дневна промяна
        printf("%s:\n", stocks[i].name);
        printf("  Сегашна цена: $%.2f\n", stocks[i].current_price);
        
        for(int day = 1; day <= FUTURE_DAYS; day++) {
            // Бъдеща цена = сегашна цена * (1 + средна дневна промяна + случайност)
            double daily_change = stocks[i].avg_daily_change/100 + 
                                ((rand() % 41) - 20) / 1000.0;
            
            predicted_price *= (1.0 + daily_change);
            predicted_change_total += daily_change * 100;
            
            if(day == 7 || day == 14) {
                printf("  Ден %2d: $%.2f (%+.1f%% от началото)\n", 
                       day, predicted_price, predicted_change_total);
            }
        }
        printf("\n");
    }
    
    printf("\nБележка: Цените са напълно случайни.\n");
    printf("Истинския маркет може също да е непредвидим!\n");
}