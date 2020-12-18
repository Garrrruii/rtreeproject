# rtreeproject
2D-R-tree code
  
2020-11-13
2D-Rtree of point object for Blockchain
count logical IO of tree-building and range/knn query processing

NOW:
  Point with integer coordinates for object, 2D-rectangel(mbr) for node
  INSERT:
    insert obj to node where need 1)smallest enlarged area 2)smallest area
  DELETE: 
    NO DELETION. b/c we don't need for this project
  SPLIT:
    divide node in half and half
    pick two seeds by choosing two nodes that makes the largest wasted space
    distribute child by choose what and where to insert with 1)smallest enlarged area 2)smallest area
  QUERYS:
    range, knn
  COUNT IO:
    ONLY for this project. Count it logically. Assume that
    1) We have nodes in DISK from level3~objects i.e. ONLY root~level2 nodes are in MEMORY
    2) Parent node can see children's coord info
    
PLAN:
  * Change it to apply to rectangular objects or real number coordinates
  * When I'm possible: visualization using python or else
    (=> maybe helpful to understand Rtree mechanism)
  * Add deletion...?
  * Add other queries? ex) window
  * Working on the project personally (Regardless of THIS project)
