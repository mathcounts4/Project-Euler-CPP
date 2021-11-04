void markPath(std::vector<std::vector<int>>& maxLength,
              std::vector<std::vector<bool>>& use,
              std::vector<std::vector<bool>>& explored,
              std::vector<std::vector<bool>>& moveD,
              std::vector<std::vector<bool>>& moveR,
              std::vector<std::vector<bool>>& moveDR,
              int x, // "down"
              int y) // "right"
{
    if (explored[x][y])
    {
        return;
    }
    explored[x][y] = true;
    
    if (x > 0 && y == 0)
    {
        moveD[x-1][y] = true;
        markPath(maxLength,use,explored,
                 moveD,moveR,moveDR,x-1,y);
    }
    else if (x == 0 && y > 0)
    {
        moveR[x][y-1] = true;
        markPath(maxLength,use,explored,
                 moveD,moveR,moveDR,x,y-1);
    }
    else if (x > 0 && y > 0)
    {
        if (use[x-1][y-1])
        {
            moveDR[x-1][y-1] = true;
            markPath(maxLength,use,explored,
                     moveD,moveR,moveDR,x-1,y-1);
        }
        if (maxLength[x-1][y] == maxLength[x][y])
        {
            moveD[x-1][y] = true;
            markPath(maxLength,use,explored,
                 moveD,moveR,moveDR,x-1,y);
        }
        if (maxLength[x][y-1] == maxLength[x][y])
        {
            moveR[x][y-1] = true;
            markPath(maxLength,use,explored,
                     moveD,moveR,moveDR,x,y-1);
        }
    }
}

std::string displayDiff(std::string oldVersion, std::string newVersion) {

    int l1 = oldVersion.length();
    int l2 = newVersion.length();
    
    std::vector<std::vector<int>>
        maxLength(l1+1,std::vector<int>(l2+1,0));
    
    std::vector<std::vector<bool>> use(l1+1,std::vector<bool>(l2+1,false));
    
    for (int i = 0; i < l1; i++)
    {
        for (int j = 0; j < l2; j++)
        {
            if (oldVersion[i] == newVersion[j])
            {
                use[i][j] = true;
                maxLength[i+1][j+1] = maxLength[i][j] + 1;
            }
            else
            {
                maxLength[i+1][j+1] = std::max(maxLength[i][j+1],maxLength[i+1][j]);
            }
        }
    }
    
    std::vector<std::vector<bool>>
        explored(l1+1,std::vector<bool>(l2+1,false)),
        moveD(l1+1,std::vector<bool>(l2+1,false)),
        moveR(l1+1,std::vector<bool>(l2+1,false)),
        moveDR(l1+1,std::vector<bool>(l2+1,false));
    
    markPath(maxLength,use,explored,moveD,moveR,moveDR,l1,l2);
    
    std::string ans;
    
    int i = 0;
    int j = 0;
    
    enum {D, R, DR} dir = DR;
    
    while (i < l1 || j < l2)
    {
        switch (dir)
        {
            case D:
                if (moveD[i][j])
                {
                    ans += oldVersion[i];
                    i++;
                }
                else
                {
                    ans += ')';
                    if (moveDR[i][j])
                    {
                        ans += oldVersion[i];
                        i++;
                        j++;
                        dir = DR;
                    }
                    else if (moveR[i][j])
                    {
                        ans += '[';
                        ans += newVersion[j];
                        j++;
                        dir = R;
                    }
                    else
                    {
                        return "";
                    }
                }
                break;
            case R:
                if (moveR[i][j])
                {
                    ans += newVersion[j];
                    j++;
                }
                else
                {
                    ans += ']';
                    if (moveDR[i][j])
                    {
                        ans += oldVersion[i];
                        i++;
                        j++;
                        dir = DR;
                    }
                    else if (moveD[i][j])
                    {
                        ans += '(';
                        ans += oldVersion[i];
                        i++;
                        dir = D;
                    }
                    else
                    {
                        return "";
                    }
                }
                break;
            case DR:
                if (moveDR[i][j])
                {
                    ans += oldVersion[i];
                    i++;
                    j++;
                    dir = DR;
                }
                
                else
                {
                    if (moveD[i][j])
                    {
                        ans += '(';
                        ans += oldVersion[i];
                        i++;
                        dir = D;
                    }
                    else if (moveR[i][j])
                    {
                        ans += '[';
                        ans += newVersion[j];
                        j++;
                        dir = R;
                    }
                    else
                    {
                        return "";
                    }
                }
                break;
        }
    }
    
    switch (dir)
    {
        case D:
            ans += ')';
            break;
        case R:
            ans += ']';
            break;
        case DR:
            break;
    }
    
    return ans;
}
