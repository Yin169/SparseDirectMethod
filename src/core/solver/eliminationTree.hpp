#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <set>

class EliminationTree {
private:
    int n;  // 节点数量
    std::vector<std::vector<int>> adj;  // 邻接表表示的图
    std::vector<std::vector<int>> etree;  // 消元树的邻接表
    std::vector<int> parent;  // 每个节点的父节点
    std::vector<int> post_order;  // 后序遍历结果

public:
    // 构造函数
    EliminationTree(int nodes) : n(nodes) {
        adj.resize(n);
        etree.resize(n);
        parent.assign(n, -1);
    }

    // 添加边（无向图）
    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    // 构建消元树的主要算法
    void buildEliminationTree() {
        std::vector<std::set<int>> ancestors(n);  // 记录每个节点的祖先集合
        
        // 按节点编号顺序进行消元
        for (int i = 0; i < n; i++) {
            // 找到节点i在当前图中的所有邻居
            std::set<int> neighbors;
            for (int neighbor : adj[i]) {
                if (neighbor < i) {  // 只考虑编号小于当前节点的邻居
                    neighbors.insert(neighbor);
                }
            }
            
            // 找到这些邻居的最小祖先作为父节点
            if (!neighbors.empty()) {
                // 合并所有邻居的祖先集合
                std::set<int> candidate_ancestors;
                for (int neighbor : neighbors) {
                    candidate_ancestors.insert(neighbor);
                    candidate_ancestors.insert(ancestors[neighbor].begin(), 
                                             ancestors[neighbor].end());
                }
                
                // 找到候选祖先中编号最大的（最近的）
                if (!candidate_ancestors.empty()) {
                    int parent_node = *candidate_ancestors.rbegin();
                    parent[i] = parent_node;
                    etree[parent_node].push_back(i);
                    
                    // 更新祖先集合
                    ancestors[i] = candidate_ancestors;
                    ancestors[i].erase(i);  // 移除自己
                }
            }
        }
    }

    // 改进版本：基于最小度数排序的消元树构建
    void buildEliminationTreeWithOrdering() {
        // 计算每个节点的度数
        std::vector<int> degree(n);
        for (int i = 0; i < n; i++) {
            degree[i] = adj[i].size();
        }
        
        // 按度数排序得到消元顺序
        std::vector<std::pair<int, int>> degree_node;
        for (int i = 0; i < n; i++) {
            degree_node.push_back({degree[i], i});
        }
        std::sort(degree_node.begin(), degree_node.end());
        
        std::vector<int> elimination_order(n);
        std::vector<int> position(n);
        for (int i = 0; i < n; i++) {
            elimination_order[i] = degree_node[i].second;
            position[degree_node[i].second] = i;
        }
        
        // 构建消元树
        std::vector<std::set<int>> ancestors(n);
        
        for (int i = 0; i < n; i++) {
            int current_node = elimination_order[i];
            
            // 找到当前节点的所有已处理邻居
            std::set<int> neighbors;
            for (int neighbor : adj[current_node]) {
                if (position[neighbor] < i) {
                    neighbors.insert(neighbor);
                }
            }
            
            // 确定父节点
            if (!neighbors.empty()) {
                // 找到最近的祖先
                std::set<int> candidate_ancestors;
                for (int neighbor : neighbors) {
                    candidate_ancestors.insert(neighbor);
                    candidate_ancestors.insert(ancestors[neighbor].begin(),
                                             ancestors[neighbor].end());
                }
                
                if (!candidate_ancestors.empty()) {
                    int parent_node = *candidate_ancestors.rbegin();
                    parent[current_node] = parent_node;
                    etree[parent_node].push_back(current_node);
                    
                    ancestors[current_node] = candidate_ancestors;
                    ancestors[current_node].erase(current_node);
                }
            }
        }
    }

    // 后序遍历
    void computePostOrder() {
        post_order.clear();
        std::vector<bool> visited(n, false);
        
        // 从根节点开始DFS
        for (int i = 0; i < n; i++) {
            if (parent[i] == -1) {  // 根节点
                postOrderDFS(i, visited);
            }
        }
    }

private:
    void postOrderDFS(int node, std::vector<bool>& visited) {
        visited[node] = true;
        
        for (int child : etree[node]) {
            if (!visited[child]) {
                postOrderDFS(child, visited);
            }
        }
        
        post_order.push_back(node);
    }

public:
    // 打印消元树
    void printEliminationTree() {
        std::cout << "Elimination Tree Structure:\n";
        for (int i = 0; i < n; i++) {
            if (!etree[i].empty()) {
                std::cout << "Node " << i << " -> Children: ";
                for (int child : etree[i]) {
                    std::cout << child << " ";
                }
                std::cout << "\n";
            }
        }
    }

    // 打印父节点信息
    void printParentInfo() {
        std::cout << "Parent Information:\n";
        for (int i = 0; i < n; i++) {
            std::cout << "Node " << i << " -> Parent: " << parent[i] << "\n";
        }
    }

    // 获取父节点
    int getParent(int node) const {
        return parent[node];
    }

    // 获取子节点
    const std::vector<int>& getChildren(int node) const {
        return etree[node];
    }

    // 获取后序遍历结果
    const std::vector<int>& getPostOrder() const {
        return post_order;
    }

    // 计算树的高度
    int getHeight() {
        std::vector<int> height(n, 0);
        std::vector<bool> visited(n, false);
        
        // 从叶子节点向上计算高度
        for (int node : post_order) {
            if (parent[node] != -1) {
                height[parent[node]] = std::max(height[parent[node]], 
                                              height[node] + 1);
            }
        }
        
        int max_height = 0;
        for (int i = 0; i < n; i++) {
            if (parent[i] == -1) {  // 根节点
                max_height = std::max(max_height, height[i]);
            }
        }
        
        return max_height;
    }
};
