#pragma once

#include <fstream>
#include <map>
#include <vector>

template <typename Symbol = char, typename Data = char,
          typename Index = unsigned>
class Suffix_automat {
private:
  class Node {
  public:
    Node(Index depth = 0) : depth_(depth) {}
    Node(Index depth, const std::map<Symbol, Index> &source)
        : next_(source), depth_(depth) {}

  public:
    bool is_way(Symbol to) const { return next_.find(to) != next_.end(); }
    Index next(Symbol to) const {
      return is_way(to) ? next_.find(to)->second : 0;
    }
    Index suffix() const { return suffix_; }
    void set_suffix(Index suffix) { suffix_ = suffix; }
    void link(Symbol to, Index position) { next_[to] = position; }
    Index depth() const { return depth_; }
    const std::map<Symbol, Index> data() const { return next_; }

  public:
    mutable Data data_;

  private:
    std::map<Symbol, Index> next_;
    Index depth_;
    Index suffix_ = 0;
  };

public:
  class State {
  public:
    State(Suffix_automat &owner, Index position)
        : owner_(owner), position_(position) {
      if (position_ >= owner.size())
        position_ = 0;
    }

  public:
    bool is_way(Symbol dir) const { return node().is_way(dir); }
    void move(Symbol dir) const { position_ = node().next(dir); }
    void move_suffix() const { position_ = node().suffix(); }
    Index position() const { return position_; }
    Data &data() const { return node().data_; }

  public:
    Node &node() { return owner_.data_[position_]; }
    Index next(Symbol to) const { return node().next(to); }
    Index suffix() const { return node().suffix(); }
    void set_suffix(Index suffix) { node().set_suffix(suffix); }
    void link(Symbol to, Index position) { node().link(to, position); }
    Index depth() const { return node().depth(); }
    void set_position(Index pos) {
      position_ = pos < owner_.data_.size() ? pos : 0;
    }

  private:
    const Node &node() const { return owner_.data_[position_]; }

  private:
    mutable Index position_;
    Suffix_automat &owner_;
  };

public:
  Suffix_automat() : state_(*this, 0) {}
  template <typename Cont> Suffix_automat(const Cont &src) : state_(*this, 0) {
    insert(src);
  }

private:
  using SA = Suffix_automat;

public:
  template <typename Iter> void insert(Iter begin, Iter end) {
    state_.set_position(0);
    for (auto iter = begin; iter != end; ++iter)
      insert(*iter);
  }
  template <typename Cont> void insert(const Cont &src) {
    insert(src.begin(), src.end());
  }
  const State root() const { return State(const_cast<SA &>(*this), 0); }
  const State current() const { return state_; }

public:
  void insert(Symbol next) {
    auto cur = size();
    data_.emplace_back(state_.depth() + 1);
    auto iter = state_;
    while (0 != iter.position() && !iter.is_way(next))
      iter.link(next, cur), iter.move_suffix();

    if (iter.position() == 0 && !iter.is_way(next)) {
      data_[cur].set_suffix(0);
      iter.link(next, cur);
    } else {
      if (!iter.is_way(next))
        iter.link(next, cur);
      auto to = iter;
      to.move(next);
      if (to.depth() == iter.depth() + 1)
        data_[cur].set_suffix(to.position());
      else {
        auto clone = size();
        data_.emplace_back(iter.depth() + 1, to.node().data());
        data_[clone].set_suffix(to.suffix());
        while (0 != iter.position() && iter.next(next) == to.position())
          iter.link(next, clone), iter.move_suffix();
        if (iter.next(next) == to.position())
          iter.link(next, clone);
        to.set_suffix(clone);
        data_[cur].set_suffix(clone);
      }
    }
    state_.set_position(cur);
  }

public:
  void dump(const std::string &filename) {
    std::ofstream file(filename);
    file << "digraph G {\nrankdir=LR;\n";
    for (Index i = 0; i < data_.size(); ++i) {
      file << " " << i << "[shape=circle label=" << i << "]\n";
      for (const auto &it : data_[i].data())
        file << "\t" << i << "->" << it.second << "[label=" << it.first
             << "]\n";
      file << i << "->" << data_[i].suffix() << "[color=lightgray]\n";
    }
    file << "}";
    file.close();
  }

public:
  size_t size() const { return data_.size(); }

private:
private:
  std::vector<Node> data_ = std::vector<Node>(1);
  State state_;
};
