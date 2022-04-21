bool oneOff(auto x, auto y)
{
    bool bad = false;
    for (int i = 0; i < x.length(); i++)
    {
        if (x[i] != y[i])
        {
            if (bad)
            {
                return false;
            }
            bad = true;
        }
    }
    return bad;
}

bool fill(auto used, auto start, auto done, auto adj)
{
    if (done == used.size())
    {
        return true;
    }
    for (int i = 0; i < used.size(); i++)
    {
        if (adj[start][i] && !used[i])
        {
            used[i] = true;
            if (fill(used,i,done+1,adj))
            {
                return true;
            }
            used[i] = false;
        }
    }
    return false;
}

bool stringsRearrangement(std::vector<std::string> inputArray) {
    int n = inputArray.size();
    std::vector<std::vector<bool>>
        adj(n,std::vector<bool>(n,false));
    
    for (int i = 0; i < n; i++)
    {
        for (int j = i+1; j < n; j++)
        {
            adj[i][j] =
                (adj[j][i] =
                 oneOff(inputArray[i],inputArray[j]));
        }
    }
    
    std::vector<bool> used(n,false);
    for (int start = 0; start < n; start++)
    {
        used[start] = true;
        if (fill(used,start,1,adj))
        {
            return true;
        }
        used[start] = false;
    }
    return false;
}
