#  schlafli.rb
#  
#  Copyright 2018 Anthony <anthony@Megatron>
#  
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#  MA 02110-1301, USA.
#  
#  

include Math

=begin
TABLE NAMES      vertices     edges      faces
icosahedron      vv           ee         ff
dodecahedron     uu           aa         bb 

    10
    / \   / \   / \   / \   / \
   /10 \ /12 \ /14 \ /16 \ /18 \
   -----1-----3-----5-----7-----9
   \ 0 / \ 2 / \ 4 / \ 6 / \ 8 / \
    \ / 1 \ / 3 \ / 5 \ / 7 \ / 9 \
     0-----2-----4-----6-----8-----
      \11 / \13 / \15 / \17 / \19 /
       \ /   \ /   \ /   \ /   \ / 
       11
=end

vv=[];ee=[];ff=[]
10.times{|i|
  vv[i]=[2*sin(PI/5*i),2*cos(PI/5*i),(-1)**i]
  ee[i]=[i,(i+1)%10];ee[i+10]=[i,(i+2)%10];ee[i+20]=[i,11-i%2]
  ff[i]=[(i-1)%10,i,(i+1)%10];ff[i+10]=[(i-1)%10,10+i%2,(i+1)%10]

}
vv+=[[0,0,-5**0.5],[0,0,5**0.5]]

uu=[];aa=[];bb=[]
10.times{|i|
  uu[i]=(0..2).map{|j|vv[ff[i][0]][j]+vv[ff[i][1]][j]+vv[ff[i][2]][j]}
  uu[i+10]=(0..2).map{|j|vv[ff[i+10][0]][j]+vv[ff[i+10][1]][j]+vv[ff[i+10][2]][j]}
  aa[i]=[i,(i+1)%10];aa[i+10]=[i,(i+10)%10];aa[i+20]=[(i-1)%10+10,(i+1)%10+10]
  bb[i]=[(i-1)%10+10,(i-1)%10,i,(i+1)%10,(i+1)%10+10] 
}
bb+=[[10,12,14,16,18],[11,13,15,17,19]]

#TABLE NAMES                           vertices     edges      faces   cells
#600 cell (analogue of icosahedron)    v            e          f       g
#120 cell (analogue of dodecahedron)   u            x          y       z 
#24 cell                               o            p          q       r

#600-CELL

# 120 vertices of 600cell. First 24 are also vertices of 24-cell

v=[[2,0,0,0],[0,2,0,0],[0,0,2,0],[0,0,0,2],[-2,0,0,0],[0,-2,0,0],[0,0,-2,0],[0,0,0,-2]]+

(0..15).map{|j|[(-1)**(j/8),(-1)**(j/4),(-1)**(j/2),(-1)**j]}+

(0..95).map{|i|j=i/12
   a,b,c,d=1.618*(-1)**(j/4),(-1)**(j/2),0.618*(-1)**j,0
   h=[[a,b,c,d],[b,a,d,c],[c,d,a,b],[d,c,b,a]][i%12/3]
   (i%3).times{h[0],h[1],h[2]=h[1],h[2],h[0]}
h}

#720 edges of 600cell. Identified by minimum distance of 2/phi between them

e=[]
120.times{|i|120.times{|j|
  e<<[i,j]  if i<j && ((v[i][0]-v[j][0])**2+(v[i][1]-v[j][1])**2+(v[i][2]-v[j][2])**2+(v[i][3]-v[j][3])**2)**0.5<1.3  
}}

#1200 faces of 600cell. 
#If 2 edges share a common vertex and the other 2 vertices form an edge in the list, it is a valid triangle.

f=[]
720.times{|i|720.times{|j|
  f<< [e[i][0],e[i][1],e[j][1]] if i<j && e[i][0]==e[j][0] && e.index([e[i][1],e[j][1]])
}}

#600 cells of 600cell.
#If 2 triangles share a common edge and the other 2 vertices form an edge in the list, it is a valid tetrahedron.

g=[]
1200.times{|i|1200.times{|j|
  g<< [f[i][0],f[i][1],f[i][2],f[j][2]] if i<j && f[i][0]==f[j][0] && f[i][1]==f[j][1] && e.index([f[i][2],f[j][2]])

}}

#120 CELL (dual of 600 cell)

#600 vertices of 120cell, correspond to the centres of the cells of the 600cell
u=g.map{|i|s=[0,0,0,0];i.each{|j|4.times{|k|s[k]+=v[j][k]/4.0}};s}

#1200 edges of 120cell at centres of faces of 600-cell. Search for pairs of tetrahedra with common face
x=f.map{|i|s=[];600.times{|j|s<<j if i==(i & g[j])};s}

#720 pentagonal faces, surrounding edges of 600-cell. Search for sets of 5 tetrahedra with common edge
y=e.map{|i|s=[];600.times{|j|s<<j if i==(i & g[j])};s}

#120 dodecahedral cells surrounding vertices of 600-cell. Search for sets of 20 tetrahedra with common vertex
z=(0..119).map{|i|s=[];600.times{|j|s<<j if [i]==([i] & g[j])};s}


#24-CELL
#24 vertices, a subset of the 600cell
o=v[0..23]

#96 edges, length 2, found by minimum distances between vertices
p=[]
24.times{|i|24.times{|j|
  p<<[i,j]  if i<j && ((v[i][0]-v[j][0])**2+(v[i][1]-v[j][1])**2+(v[i][2]-v[j][2])**2+(v[i][3]-v[j][3])**2)**0.5<2.1  
}}

#96 triangles
#If 2 edges share a common vertex and the other 2 vertices form an edge in the list, it is a valid triangle.
q=[]
96.times{|i|96.times{|j|
  q<< [p[i][0],p[i][1],p[j][1]] if i<j && p[i][0]==p[j][0] && p.index([p[i][1],p[j][1]])
}}


#24 cells. Calculates the centre of the cell and the 6 vertices nearest it
r=(0..23).map{|i|a,b=(-1)**i,(-1)**(i/2)
    c=[[a,b,0,0],[a,0,b,0],[a,0,0,b],[0,a,b,0],[0,a,0,b],[0,0,a,b]][i/4]
    s=[]
    24.times{|j|t=v[j]
    s<<j if (c[0]-t[0])**2+(c[1]-t[1])**2+(c[2]-t[2])**2+(c[3]-t[3])**2<=2 
    }
s}


tetr=->n{

  #Tetrahedron Family Vertices
  tv=(0..n).map{|i|
    s=[0]*n
    if i==n
      s.map!{(1-(1+n)**0.5)}
    else
      s[i]+=n
    end
    s.map!{|j|j-((1-(1+n)**0.5)+n)/(1+n)}
  s}

  #Tetrahedron Family Graph
  tg=(0..n+1).map{[]}
  (2**(n+1)).times{|i|
    s=[]
    (n+1).times{|j|s<<j if i>>j&1==1}
    tg[s.size]<<s
  }

return [tv]+tg[2..n]}

cube=->n{

  #Cube Family Vertices
  cv=(0..2**n-1).map{|i|s=[];n.times{|j|s<<(i>>j&1)*2-1};s}

  #Cube Family Graph
  cg=(0..n+1).map{[]}
  (3**n).times{|i|                         #for each point
    s=[]
    cv.size.times{|j|                      #and each vertex
      t=true                               #assume vertex goes with point
      n.times{|k|                          #and each pair of opposite sides
        t&&= (i/(3**k)%3-1)*cv[j][k]!=-1   #if the vertex has kingsmove distance >1 from point it does not belong      
      }
      s<<j if t                            #add the vertex if it belongs
    }
    cg[log2(s.size)+1]<<s if s.size > 0
  } 

return [cv]+cg[2..n]}

octa=->n{

  #Octahedron Family Vertices
  ov=(0..n*2-1).map{|i|s=[0]*n;s[i/2]=(-1)**i;s}

  #Octahedron Family Graph
  og=(0..n).map{[]}
  (3**n).times{|i|                         #for each point
    s=[]
    ov.size.times{|j|                      #and each vertex
      n.times{|k|                          #and each pair of opposite sides
        s<<j if (i/(3**k)%3-1)*ov[j][k]==1 #if the vertex is located in the side corresponding to the point, add the vertex to the list      
      }    
    }
    og[s.size]<<s
  } 

return [ov]+og[2..n]}

polytope=->schl{
  if schl.size==1                                #if a single digit calculate and return a polygon
    return [(1..schl[0]).map{|i|[sin(PI*2*i/schl[0]),cos(PI*2*i/schl[0])]},(1..schl[0]).map{|i|[i%schl[0],(i+1)%schl[0]]}]  
  elsif  i=[[3,5],[5,3]].index(schl)             #if a 3d special, lookup from tables
    return [[vv,ee,ff],[uu,aa,bb]][i]
  elsif i=[[3,3,5],[5,3,3],[3,4,3]].index(schl)  #if a 4d special. lookup fromm tables
    return [[v,e,f,g],[u,x,y,z],[o,p,q,r]][i]
  elsif schl.size==schl.count(3)                 #if all threes, call tetr for a hypertetrahedron
    return tetr[schl.size+1]
  elsif schl.size-1==schl.count(3)               #if all except one number 3
    return cube[schl.size+1] if schl[0]==4       #and the 1st digit is 4, call cube for a hypercube
    return octa[schl.size+1] if schl[-1]==4      #and the last digit is 4, call octa for a hyperoctahedron
  end
  return "error"                                 #in any other case return an error
}


cell24 = polytope[[3,4,3]]

puts "vertices"
cell24[0].each{|i|p i}
puts "edges"
cell24[1].each{|i|p i}
puts "faces"
cell24[2].each{|i|p i}
puts "cells"
cell24[3].each{|i|p i}

