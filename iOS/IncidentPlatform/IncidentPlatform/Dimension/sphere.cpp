#include "sphere.h"

RESULT sphere::SetSphere()
{
    RESULT r = R_SUCCESS;
    int c = 0;
    float AngleY = 0;
    float AngleY1 = 0;
    float AngleZ = 0;
    float AngleZ1 = 0;
    int cVert = 0;
    int vAng = 0;
        
    float x, y, z;
    float x1, y1, z1;
    float x2, z2;
    float x3, z3;

    // Start from Bottom
    
    // Create Bottom Fan
    m_verts[0].set(point(0, -m_fRadius, 0));
    m_verts[0].SetNormal(0.0f, -1.0f, 0.0f);

    cVert++;
    c++;

    AngleZ = (PI)*((float)cVert/(float)(m_iVerticalDivisions + 1));
    y = -m_fRadius*cos(AngleZ);
    
    for(int i = m_iAngularDivisions; i >= 0; i--)
    {
        AngleY = (2*PI)*((float)i/(float)(m_iAngularDivisions));
        // First
        x = m_fRadius*sin(AngleY)*sin(AngleZ);
        z = m_fRadius*cos(AngleY)*sin(AngleZ);

        vector v(x, y, z);

        //m_verts[c].set(m_ptOrigin + v);
        m_verts[c].set(v);
        v.normalize();
        m_verts[c].SetNormal(v);
        c++;
    }
    
    for(int j = 0; j < m_iVerticalDivisions - 1; j++)
    {
        AngleZ = (PI)*((float)cVert/(float)(m_iVerticalDivisions + 1));
        AngleZ1 = (PI)*((float)(cVert + 1)/(float)(m_iVerticalDivisions + 1));
        y = -m_fRadius*cos(AngleZ);
        y1 = -m_fRadius*cos(AngleZ1);

        // Create the strips
        for(int i = 0; i < m_iAngularDivisions; i++)
        {
            AngleY = (2*PI)*((float)i/(float)(m_iAngularDivisions));
            AngleY1 = (2*PI)*((float)(i + 1)/(float)(m_iAngularDivisions));

            // First
            x = m_fRadius*sin(AngleY)*sin(AngleZ);
            z = m_fRadius*cos(AngleY)*sin(AngleZ);
            // Top
            x1 = m_fRadius*sin(AngleY)*sin(AngleZ1);
            z1 = m_fRadius*cos(AngleY)*sin(AngleZ1);
            // Right
            x2 = m_fRadius*sin(AngleY1)*sin(AngleZ);
            z2 = m_fRadius*cos(AngleY1)*sin(AngleZ);
            // Top Right
            x3 = m_fRadius*sin(AngleY1)*sin(AngleZ1);
            z3 = m_fRadius*cos(AngleY1)*sin(AngleZ1);

            vector v(x, y, z);
            vector v1(x1, y1, z1);
            vector v2(x2, y, z2);
            vector v3(x3, y1, z3);

            // First
            //m_verts[c].set(m_ptOrigin + v);
            m_verts[c].set(v);
            v.normalize();
            m_verts[c].SetNormal(v);
            // Top
            //m_verts[c + 2].set(m_ptOrigin + v1);
            m_verts[c + 2].set(v1);
            v1.normalize();
            m_verts[c + 2].SetNormal(v1);
            // Right
            //m_verts[c + 1].set(m_ptOrigin + v2);
            m_verts[c + 1].set(v2);
            v2.normalize();
            m_verts[c + 1].SetNormal(v2);
            // Top Right
            //m_verts[c + 3].set(m_ptOrigin + v3);
            m_verts[c + 3].set(v3);
            v3.normalize();
            m_verts[c + 3].SetNormal(v3);
            c += 4;
        }
        cVert++;
    }
    // Create Top Fan
    //m_verts[c].set(m_ptOrigin + point(0, m_fRadius, 0));
    m_verts[c].set(point(0, m_fRadius, 0));
    m_verts[c].SetNormal(0.0f, 1.0f, 0.0f);
    c++;

    AngleZ = (PI)*((float)cVert/(float)(m_iVerticalDivisions + 1));
    y = -m_fRadius*cos(AngleZ);
    
    for(int i = 0; i <= m_iAngularDivisions; i++)
    {
        AngleY = (2*PI)*((float)i/(float)(m_iAngularDivisions));
        // First
        x = m_fRadius*sin(AngleY)*sin(AngleZ);
        z = m_fRadius*cos(AngleY)*sin(AngleZ);

        vector v(x, y, z);

        //m_verts[c].set(m_ptOrigin + v);
        m_verts[c].set(v);
        v.normalize();
        m_verts[c].SetNormal(v);
        c++;
    }

Error:
    return r;
}

RESULT sphere::SetIndexedSphere()
{
    RESULT r = R_SUCCESS;

    int VertCounter = 0;

    // Set up the verts
    m_cVerts = 1 + // bottom vertex
               m_iVerticalDivisions * (m_iAngularDivisions + 1) + // Indexed vertices
               1;   // Top Vertex
    m_verts = new vertex[m_cVerts];

    // Create the bottom vertex
    m_verts[0].set(point(0, m_fRadius, 0));
    m_verts[0].SetNormal(0.0f, 1.0f, 0.0f);
    m_verts[0].SetUV(0.0f, 0.0f);
    VertCounter++;

    for(int i = 0; i < m_iVerticalDivisions; i++)
    {
        float AngleZ = (PI * m_LongSize)*((float)(i + 1)/(float)(m_iVerticalDivisions + 1));
        float y = m_fRadius*cos(AngleZ);

        for(int j = 0; j < m_iAngularDivisions + 1; j++)
        {
            float AngleY = (2*PI*m_LatSize)*((float)j/(float)(m_iAngularDivisions));
            float x = m_fRadius * sin(AngleY) * sin(AngleZ);
            float z = m_fRadius * cos(AngleY) * sin(AngleZ);
            vector n(x, y, z);
            n.normalize();
    
            m_verts[VertCounter].set(x, y, z);
            m_verts[VertCounter].SetNormal(n);

            // procedurally create the tangent vectors
            // since the binormal will be <n_x, -n_y, n_z>
            
            vector Binormal(vX(n), -1.0f * vY(n), vZ(n));
            vector Tangent;
            if(AngleZ < PI/2)
            {            
                Tangent = n % Binormal;// % n;
            }
            else if(AngleZ > PI/2)
            {
                Tangent = Binormal % n;
            }
            else
            {
                Binormal = vector(0.0f, -1.0f, 0.0f);
                Tangent = Binormal % n;
            }
            m_verts[VertCounter].SetTangent(Tangent);

            float U = AngleY / (2 * PI * m_LatSize);
            float V = AngleZ / (PI * m_LongSize);

            m_verts[VertCounter].SetUV(U, V);

            VertCounter++;
        }
    }

    // Create the top vertex
    m_verts[m_cVerts - 1].set(point(0, -m_fRadius, 0));
    m_verts[m_cVerts - 1].SetNormal(0.0f, -1.0f, 0.0f);
    m_verts[m_cVerts - 1].SetUV(0.0f, 1.0f);

    // Populate the Tangent Vectors
    // For top/bottom verts we arbitrarily choose a tangent vector perpendicular to the normal
    // which we have previously defined
    m_verts[0].SetTangent(1.0f, 0.0f, 0.0f);
    m_verts[m_cVerts - 1].SetTangent(-1.0f, 0.0f, 0.0f);    // flip it since it's upside down!

    /*
    for(int i = 0; i < m_iVerticalDivisions; i++)
    {
        for(int j = 0; j < m_iAngularDivisions + 1; j++)
        {
            int Index = 1 + i * (m_iAngularDivisions + 1) + j;  // current index
            
            int IndexYInc; 
            if(j == m_iAngularDivisions)
            {
                IndexYInc = 1 + (i) * (m_iAngularDivisions + 1) + 0;   // back to the first one of this "row"
            }
            else
            {
                IndexYInc = 1 + (i) * (m_iAngularDivisions + 1) + j + 1;   // normal style
            }

            int IndexZInc;
            if(i == m_iVerticalDivisions - 1)
            {
                IndexZInc = m_cVerts - 1;       // If we're on the last row next is the last vertex
            }
            else
            {
                IndexZInc = 1 + (i + 1) * (m_iAngularDivisions + 1) + j + 1;
            }

            vector v1 = m_verts[IndexYInc] - m_verts[Index];
            vector v2 = m_verts[IndexZInc] - m_verts[Index];

            float dU1 = m_verts[IndexYInc].m_UV.tuple[0] - m_verts[Index].m_UV.tuple[0];
            float dU2 = m_verts[IndexZInc].m_UV.tuple[0] - m_verts[Index].m_UV.tuple[0];

            /*
            // NEED TO ADD SPECIAL HANDLING FOR SEAM
            if(j == m_iAngularDivisions)   
            {
                dU1 = m_verts[IndexYInc - 1].m_UV.tuple[0] - m_verts[Index - 1].m_UV.tuple[0];
                //dU2 = m_verts[IndexZInc].m_UV.tuple[0] - m_verts[Index - 1].m_UV.tuple[0];
            } 
            

            vector Tangent;
            if(dU1 != 0)
            {
                Tangent = v1 * (1.0f / dU1);
            }
            else if(dU2 != 0)
            {
                Tangent = v2 * (1.0f / dU2);
            }
            else
            {
                printf("ERROR!: Cannot map tangent vector error in mesh or something\n");
            }
            
                  
            Tangent.normalize();
            m_verts[Index].SetTangent(Tangent);
        }
    }
    */

    // Set up the indices
    m_indices_n = 2 * (2 + m_iAngularDivisions) + // bottom / top fans
                  ((m_iVerticalDivisions - 1) * (m_iAngularDivisions) * 6); // strips
    m_indices = new int[m_indices_n];
    memset(m_indices, 0, m_indices_n * sizeof(int));

    // Set up bottom triangle strip
    
    for(int i = 0; i < m_iAngularDivisions + 2; i++)
    {
        m_indices[i] = i;
    }
    
    /*if(m_LatSize == 1.0f)
        m_indices[m_iAngularDivisions + 1] = 1;*/
   
    // Now we have m_iVerticalDivisions - 3 triangle strips
    for(int i = 0; i < m_iVerticalDivisions - 1; i++)
    {        
        for(int j = 0; j < m_iAngularDivisions; j++)
        {            
            int BaseIndex = (m_iAngularDivisions * i) * 6 + m_iAngularDivisions + 2 + j * 6;
            
            m_indices[BaseIndex + 2] = (i * (m_iAngularDivisions + 1)) + j + 1;
            m_indices[BaseIndex + 1] = (i * (m_iAngularDivisions + 1)) + (j + 1) + 1;
            m_indices[BaseIndex + 0] = ((i + 1) * (m_iAngularDivisions + 1)) + (j + 1) + 1;                       
            
            m_indices[BaseIndex + 3] = ((i + 1) * (m_iAngularDivisions + 1)) + (j + 1) + 1;
            m_indices[BaseIndex + 4] = (i * (m_iAngularDivisions + 1)) + j + 1;
            m_indices[BaseIndex + 5] = ((i + 1) * (m_iAngularDivisions + 1)) + j + 1;           
        }

        /*
        if(m_LatSize == 1.0f)
        {
            int BaseIndex = (m_iAngularDivisions * i) * 6 + (m_iAngularDivisions + 2) + (m_iAngularDivisions - 1) * 6;

            m_indices[BaseIndex + 2] = (i * m_iAngularDivisions) + (m_iAngularDivisions - 1) + 1;
            m_indices[BaseIndex + 1] = (i * m_iAngularDivisions) + 1;
            m_indices[BaseIndex + 0] = ((i + 1) * m_iAngularDivisions) + 1;                       

            m_indices[BaseIndex + 3] = ((i + 1) * m_iAngularDivisions) + 1;
            m_indices[BaseIndex + 4] = (i * m_iAngularDivisions) + (m_iAngularDivisions - 1) + 1;
            m_indices[BaseIndex + 5] = ((i + 1) * m_iAngularDivisions) + (m_iAngularDivisions - 1) + 1;
        }
*/
    }
    

    int j = m_cVerts - 1;
    for(int i = (2 + m_iAngularDivisions) + ((m_iVerticalDivisions - 1) * (m_iAngularDivisions) * 6); i < m_indices_n - 1; i++)
    {
        m_indices[i] = j;
        j--;
    }
    m_indices[m_indices_n - 1] = m_cVerts - 2;

Error:
    return r;
}