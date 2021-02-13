#pragma once

class Y {
  private:
    int value;
  public:
    Y(int v);
    void set_value(int new_value);
    int get_value() const;
};
