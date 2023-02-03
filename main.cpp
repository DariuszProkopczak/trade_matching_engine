#include <map>
#include <set>
#include <list>
#include <cmath>
#include <ctime>
#include <deque>
#include <queue>
#include <stack>
#include <string>
#include <bitset>
#include <cstdio>
#include <limits>
#include <vector>
#include <climits>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <numeric>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <unordered_map>

using namespace std;

struct Order {
    int price;
    int quantity;
    string order_id;
};

class OrderBook {
    list<Order> buy_orders;
    list<Order> sell_orders;
    unordered_map<string, int> active_order_ids;
    
    public:
    
    int execute_buy_order(int price, int quantity, string order_id, int modifying) {
        // Executes a buy order using best available sell orders.
        // Returns the quantity of items that were not bought.
        // "modifying" specifies whether this order exists already.
        
        // check for duplicate order id if not modifying current order
        if (modifying == 0)
        {
            if (active_order_ids.find(order_id) != active_order_ids.end())
                return 0;
        }

        for (list<Order>::iterator sell_order = sell_orders.begin(); sell_order != sell_orders.end(); ++sell_order) {
            if (quantity == 0)
                // buy order was completely filled
                break;
            if (sell_order->price > price) 
                // remaining sell orders are priced higher than the buy order
                break;

            if (sell_order->quantity <= quantity) {
                // Current sell order quantity is less than or equal to
                // remaining buy quantity. Will buy the whole order.
                cout << "TRADE " << sell_order->order_id
                     << " " << sell_order->price << " " << sell_order->quantity
                     << " " << order_id
                     << " " << price << " " << sell_order->quantity << endl;
                quantity -= sell_order->quantity;
                // sell order can be removed
                active_order_ids.erase(sell_order->order_id);
                sell_orders.erase(sell_order);
            } 
            else {
                // Current sell order quantity is greater than remaining buy 
                // quantity.
                cout << "TRADE " << sell_order->order_id
                     << " " << sell_order->price << " " << quantity
                     << " " << order_id
                     << " " << price << " " << quantity << endl;
                sell_order->quantity -= quantity;
                quantity = 0;
            }
        }
        
        return quantity;
    }
    
    int execute_sell_order(int price, int quantity, string order_id, int modifying) {
        // Executes a sell order using best available buy orders.
        // Returns the quantity of items that were not sold.
        //  "modifying" specifies whether this order exists already.
        
        // check for duplicate order id if not modifying current order
        if (modifying == 0)
        {
            if (active_order_ids.find(order_id) != active_order_ids.end())
                return 0;
        }

        for (list<Order>::iterator buy_order = buy_orders.begin(); buy_order != buy_orders.end(); ++buy_order) {
            if (quantity == 0)
                // sell order was completely filled
                break;
            if (buy_order->price < price) 
                // remaining buy orders are priced lower than the sell order
                break;

            if (buy_order->quantity <= quantity) {
                // Current buy order quantity is less than or equal to
                // remaining sell quantity. Will execute whole buy order.
                cout << "TRADE " << buy_order->order_id
                     << " " << buy_order->price << " " << buy_order->quantity
                     << " " << order_id
                     << " " << price << " " << buy_order->quantity << endl;
                quantity -= buy_order->quantity;
                // buy order can be removed
                active_order_ids.erase(buy_order->order_id);
                buy_orders.erase(buy_order);
            } 
            else {
                // Current buy order quantity is greater than remaining sell 
                // quantity.
                cout << "TRADE " << buy_order->order_id
                     << " " << buy_order->price << " " << quantity
                     << " " << order_id
                     << " " << price << " " << quantity << endl;
                buy_order->quantity -= quantity;
                quantity = 0;
            }
        }
        
        return quantity;
    }
    
    void add_buy_order(int price, int quantity, string order_id, int modifying) {
        // Add buy order to order book. Executes order if matching sell orders
        // exist. "modifying" specifies whether this order exists already.
        
        // execute any available trades before adding order
        quantity = execute_buy_order(price, quantity, order_id, modifying);
        
        // add any remaining quantity to order book
        if (quantity > 0) {
            insert_buy_order(buy_orders.begin(), price, quantity, order_id);
        }        
    }
    
    void add_sell_order(int price, int quantity, string order_id, int modifying) {
        // Add sell order to order book. Executes order if matching buy orders
        // exist. "modifying" specifies whether this order exists already.

        // execute any available trades before adding order
        quantity = execute_sell_order(price, quantity, order_id, modifying);

        // add any remaining quantity to order book
        if (quantity > 0) {
            insert_sell_order(sell_orders.begin(), price, quantity, order_id);
        }        
    }
    
    void insert_buy_order(list<Order>::iterator start_buy_order, int price, int quantity, string order_id) {
        // insert buy order after a specified starting position in the order book
        
        Order new_order;
        int order_added = 0;
        new_order.price = price;
        new_order.quantity = quantity;
        new_order.order_id = order_id;
        for (list<Order>::iterator curr_buy_order = start_buy_order; curr_buy_order != buy_orders.end(); ++curr_buy_order) {
            if (curr_buy_order->price < price)
            {
                buy_orders.insert(curr_buy_order, new_order);
                order_added = 1;
                break;
            }
        }
        
        if (order_added == 0)
            buy_orders.push_back(new_order);
            
        active_order_ids.insert({new_order.order_id, 1});
    }
    
    void insert_sell_order(list<Order>::iterator start_sell_order, int price, int quantity, string order_id) {
        // insert sell order after a specified starting position in the order book
        
        Order new_order;
        int order_added = 0;
        new_order.price = price;
        new_order.quantity = quantity;
        new_order.order_id = order_id;
        for (list<Order>::iterator curr_sell_order = start_sell_order; curr_sell_order != sell_orders.end(); ++curr_sell_order) {
            if (curr_sell_order->price > price)
            {
                sell_orders.insert(curr_sell_order, new_order);
                order_added = 1;
                break;
            }
        }
        
        if (order_added == 0)
            sell_orders.push_back(new_order);
            
        active_order_ids.insert({new_order.order_id, 1});
    }
    
    void cancel_order(string order_id) {
        // cancels order specified by order_id
        
        // check for duplicate order id
        if (active_order_ids.find(order_id) == active_order_ids.end())
            return;
        
        // search for order id in sell orders
        int order_deleted = 0;
        for (list<Order>::iterator sell_order = sell_orders.begin(); sell_order != sell_orders.end(); ++sell_order) {
            if (sell_order->order_id.compare(order_id) == 0) {
                active_order_ids.erase(sell_order->order_id);
                sell_orders.erase(sell_order); 
                order_deleted = 1;
                break;  
            }
        }
        
        // search for order id in buy orders
        if (order_deleted == 0) {
            for (list<Order>::iterator buy_order = buy_orders.begin(); buy_order != buy_orders.end(); ++buy_order) {
                if (buy_order->order_id.compare(order_id) == 0) {
                    active_order_ids.erase(buy_order->order_id);
                    buy_orders.erase(buy_order); 
                    break;  
                }
            }
        }
    }
    
    void modify_order(string order_id, int buy_or_sell, int price, int quantity) {
        // Modify an order. "buy_or_sell" represents sell if it is 0, and buy if it is 1.
        
        // check for duplicate order id
        if (active_order_ids.find(order_id) == active_order_ids.end())
            return;
        
        // search for order id in sell orders
        int order_found = 0;
        for (list<Order>::iterator sell_order = sell_orders.begin(); sell_order != sell_orders.end(); ++sell_order) {
            if (sell_order->order_id.compare(order_id) == 0) {
                order_found = 1;
                if (buy_or_sell == 0) {
                    sell_order = sell_orders.erase(sell_order);
                    // insert_sell_order(sell_orders.begin(), price, quantity, order_id);
                    add_sell_order(price, quantity, order_id, 1);
                } else {
                    cancel_order(order_id);
                    add_buy_order(price, quantity, order_id, 1);
                }
                break;
            }
        }
        
        // search for order id in buy orders
        if (order_found == 0) {
            for (list<Order>::iterator buy_order = buy_orders.begin(); buy_order != buy_orders.end(); ++buy_order) {
                if (buy_order->order_id.compare(order_id) == 0) {
                    if (buy_or_sell == 1) {
                        buy_order = buy_orders.erase(buy_order);
                        // insert_buy_order(buy_orders.begin(), price, quantity, order_id);
                        add_buy_order(price, quantity, order_id, 1);
                    } else {
                        cancel_order(order_id);
                        add_sell_order(price, quantity, order_id, 1);
                    }
                    break;
                }
            }
        }
    }
    
    void print_order_books() {
        // prints sell and buy order books

        int curr_price = -1;
        int curr_quantity_sum;
        
        // print sell orders
        cout << "SELL:" << endl;
        for (list<Order>::reverse_iterator sell_order = sell_orders.rbegin(); sell_order != sell_orders.rend(); ++sell_order) {
            if (sell_order == sell_orders.rbegin()) {
                curr_price = sell_order->price;
                curr_quantity_sum = sell_order->quantity;
            } 
            else if (sell_order->price == curr_price) {
                curr_quantity_sum += sell_order->quantity;
            }
            else {
                cout << curr_price << " " << curr_quantity_sum << endl;
                curr_price = sell_order->price;
                curr_quantity_sum = sell_order->quantity;
            }
        }
        if (curr_price != -1) {
            cout << curr_price << " " << curr_quantity_sum << endl;
            curr_price = - 1;
        }

        // print buy orders
        cout << "BUY:" << endl;
        for (list<Order>::iterator buy_order = buy_orders.begin(); buy_order != buy_orders.end(); ++buy_order) {
            if (buy_order == buy_orders.begin()) {
                curr_price = buy_order->price;
                curr_quantity_sum = buy_order->quantity;
            } 
            else if (buy_order->price == curr_price) {
                curr_quantity_sum += buy_order->quantity;
            }
            else {
                cout << curr_price << " " << curr_quantity_sum << endl;  
                curr_price = buy_order->price;
                curr_quantity_sum = buy_order->quantity;
            }
        }
        if (curr_price != -1) {
            cout << curr_price << " " << curr_quantity_sum << endl;
            curr_price = - 1;
        }
    }  
};

int get_int_token(string * line, string delimiter, size_t * pos) {
    *pos = line->find(delimiter);
    int token = stoi(line->substr(0, *pos));
    line->erase(0, *pos + delimiter.length());

    return token;
}

string get_string_token(string * line, string delimiter, size_t * pos) {
    *pos = line->find(delimiter);
    string token = line->substr(0, *pos);
    line->erase(0, *pos + delimiter.length());

    return token;
}

int main() {
    
    OrderBook order_book;
    
    string line;
    size_t pos;
    string delimiter = " ";
    string token;
    string ioc_or_gfd;
    string buy_or_sell;
    int price;
    int quantity;
    string order_id;

    while (getline(cin, line))
    {
        if (line.empty()) {
            break;
        }
 
        if (line[0] != '\0')
        {
            token = get_string_token(&line, delimiter, &pos);
            
            if (token.compare("BUY") == 0) {

                ioc_or_gfd = get_string_token(&line, delimiter, &pos);
                price = get_int_token(&line, delimiter, &pos);
                quantity = get_int_token(&line, delimiter, &pos);
                order_id = get_string_token(&line, delimiter, &pos);
           
                if ((price <= 0) || (quantity <= 0) || (order_id.length() == 0)) 
                    continue;
                
                if (ioc_or_gfd.compare("GFD") == 0)
                    order_book.add_buy_order(price, quantity, order_id, 0);
                else
                    order_book.execute_buy_order(price, quantity, order_id, 0);
            }
            else if (token.compare("SELL") == 0) {
                ioc_or_gfd = get_string_token(&line, delimiter, &pos);
                price = get_int_token(&line, delimiter, &pos);
                quantity = get_int_token(&line, delimiter, &pos);
                order_id = get_string_token(&line, delimiter, &pos);
                
                if ((price <= 0) || (quantity <= 0) || (order_id.length() == 0)) 
                    continue;
                
                if (ioc_or_gfd.compare("GFD") == 0)
                    order_book.add_sell_order(price, quantity, order_id, 0);
                else
                    order_book.execute_sell_order(price, quantity, order_id, 0);
            }                
            else if (token.compare("CANCEL") == 0) {

                order_id = get_string_token(&line, delimiter, &pos);
                
                if (order_id.length() == 0)
                    continue;
                
                order_book.cancel_order(order_id);
            }
            else if (token.compare("MODIFY") == 0) {

                order_id = get_string_token(&line, delimiter, &pos);
                buy_or_sell = get_string_token(&line, delimiter, &pos);
                price = get_int_token(&line, delimiter, &pos);
                quantity = get_int_token(&line, delimiter, &pos);
                
                if ((price <= 0) || (quantity <= 0) || (order_id.length() == 0)) 
                    continue;
                
                if (buy_or_sell.compare("SELL") == 0)
                    order_book.modify_order(order_id, 0, price, quantity);
                else 
                    order_book.modify_order(order_id, 1, price, quantity);
            }
            else if (token.compare("PRINT") == 0) {
                    order_book.print_order_books();
            }
        }
    }

    return 0;
}