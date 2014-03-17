/**
 * @file kdtree.cpp
 * Implementation of KDTree class.
 */

template<int Dim>
bool KDTree<Dim>::smallerDimVal(const Point<Dim> & first, const Point<Dim> & second, int curDim) const
{
	if(first[curDim] == second[curDim])
		return first < second;

	return first[curDim] < second[curDim];
}


template<int Dim>
bool KDTree<Dim>::shouldReplace(const Point<Dim> & target, const Point<Dim> & currentBest, const Point<Dim> & potential) const
{
	int sumCB = 0;
	int sumPot = 0;
	for(int i = 0; i < Dim; i++)
	{
		sumCB += pow((target[i] - currentBest[i]), 2);
		sumPot += pow((target[i] - potential[i]), 2);
	}

	if(sumPot == sumCB)
		return potential<currentBest;
    return sumPot<sumCB;
}


template<int Dim>
KDTree<Dim>::KDTree(const vector< Point<Dim> > & newPoints)
{
	if(newPoints.size() != 0)
	{
		points.resize(newPoints.size());
		for(int i = 0; i < points.size(); i++)
			points[i] = newPoints[i];
		int max = points.size() - 1;
		int min = 0;
		make(points, min, max, 0);
	}

}
	

template<int Dim>
void KDTree<Dim>::make(vector< Point<Dim> > & newPoints, int mini, int maxi, int curDim)
{
	int root = (maxi + mini)/2;

	if(maxi > mini);
	{
		select(newPoints, mini, maxi, root, curDim);
		if( mini < root)
			make(newPoints, mini, root - 1, (curDim + 1)%Dim);
		if(maxi > root)
			make(newPoints, root + 1, maxi, (curDim + 1)%Dim);

	}
}

template<int Dim>
void KDTree<Dim>::select(vector< Point<Dim> > & newPoints, int left, int right, int n, int curDim)
{
	if(left >= right)
		return;
	while(left != right)
	{

		int pivotIndex = (left + right)/2;
		int newPiv = partition(newPoints, left, right, pivotIndex, curDim);
		if(n == newPiv)
			return;		
		else if(n < newPiv)
			right = newPiv - 1;
		else
		{
			left = newPiv + 1;
		}
	}
}

template<int Dim>
int KDTree<Dim>::partition(vector< Point<Dim> > & newPoints, int left, int right, int pivot, int curDim)
{
	if(!newPoints.empty())
	{	
		Point<Dim> pivVal = newPoints[pivot];
		Point<Dim> temp = newPoints[pivot];
		newPoints[pivot] = newPoints[right];
		newPoints[right] = temp;

		int storeIndex = left; //storeIndex initialized to left
		
		for(int i = left; i < right; i++) 
		{
			if(smallerDimVal(newPoints[i], pivVal, curDim))
			{
				temp = newPoints[storeIndex]; 
				newPoints[storeIndex] = newPoints[i];
				newPoints[i] = temp; 
				storeIndex++; 
			}
		}

		temp = newPoints[right];
		newPoints[right] = newPoints[storeIndex];
		newPoints[storeIndex] = temp; //swap storeIndex with right
	
		return storeIndex;
	}

	else
		return 0;
}


template<int Dim>
Point<Dim> KDTree<Dim>::findNearestNeighbor(const Point<Dim> & query) const
{
	return find(query, 0, (points.size() - 1), 0);
}

template<int Dim>
Point<Dim> KDTree<Dim>::find(const Point<Dim> & query, int min, int max, int curDim) const
{
	int mid = (min + max)/2;
	bool inLeft = false;
	Point<Dim> currentBest = points[mid];

	if(smallerDimVal(query, points[mid], curDim))
	{
		inLeft = true;
		if(min == mid)
			currentBest = points[mid]; 
		else
			currentBest = find(query, min, mid - 1, (curDim + 1) % Dim);
	}

	else
	{
		if(max == mid)
			currentBest = points[mid]; 

		else
			currentBest = find(query, mid + 1, max, (curDim + 1) % Dim);
	}

	Point<Dim> potential = points[mid];

	if(shouldReplace(query, currentBest, potential))
			currentBest = potential;

	int distance = 0;

	for(int i = 0; i < Dim; i++)
		distance += pow(currentBest[i] - query[i], 2);
				
	int radius = 0;
	radius += pow((query[curDim] - points[mid][curDim]), 2);

	if((radius <= distance) && (inLeft) && (max > mid))
			potential = find(query, mid + 1, max, (curDim + 1) % Dim);

	else if((radius <= distance) && (!inLeft) && (min < mid))
			potential = find(query,min, mid - 1, (curDim + 1) % Dim);

	if(shouldReplace(query, currentBest, potential))
			currentBest = potential;

	return currentBest;
}


