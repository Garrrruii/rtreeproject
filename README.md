# Rtree building and query processing
2D-R-tree code
  
2020-11-13
* 2D-Rtree of point object for blockchain => no need deletion
* count logical IO of tree-building and range/knn query processing => node access count

NOW:
* Point with integer coordinates for object, 2D-rectangle(mbr)---node
* INSERT:
  insert obj to node with 1)smallest enlarged area 2)smallest area
* DELETE: 
  NO DELETION.
* SPLIT:
  * divide node in half and half
  * pick two seeds by choosing two nodes making largest wasted space
  * distribute child by choosing what and where to insert with 1)smallest enlarged area 2)smallest area
* QUERYS:
  range, knn
* COUNT IO:
  ONLY for this project. Count it logically. Assume that
  1) We have nodes in DISK from lv3 i.e. ONLY root~lv2 nodes are in MEMORY
  2) Parent node can see children's coord info
    
PLAN:
* Change code using 'Class'
* Change it to apply to rectangular objects or real number coordinates
* If it's possible: visualization using python or else
  (something helpful to understand Rtree mechanism)
* Add deletion...?
* Add other queries? ex) window, rnn
* Else, working on the project personally (Regardless of THIS project)
