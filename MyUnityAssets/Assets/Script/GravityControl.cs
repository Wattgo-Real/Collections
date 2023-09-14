using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GravityControl : MonoBehaviour
{
    public GameObject[] TargetGravity;
    public GameObject DefaultPlaceOnCamera;
    public Camera cam;

    public float maxSpeed;
    public float mixAccelerate;
    public float mouseSpeed = 1;
    public float jumpHight = 2;
    public float jumpAngleLimet = 70f;
    public bool airMove = true;
    float plainRotateX;
    float plainRotateY;
    bool onGround = true;
    bool inJump = false;

    Vector3 desiredVelocity;
    Vector3 velocity;
    Vector3 forW;

    Vector3 stepRotrte,stepPosition;

    Rigidbody rig;

    public Vector3 CTP()
    {
        Vector3 cameraTargetPoint = Vector3.zero;
        Vector3 L = Vector3.zero;
        for(int i=0;i<TargetGravity.Length;i++)
        {
            L += TargetGravity[i].transform.position - gameObject.transform.position;
        }
        cameraTargetPoint = gameObject.transform.position + L.normalized;
        return cameraTargetPoint;
    }

    void Start()
    {
        Cursor.lockState = CursorLockMode.Locked;
        rig = GetComponent<Rigidbody>();
        //設置初始角度(不重要)
        stepRotrte = cam.transform.rotation.eulerAngles;
        //設置初始位置(重要)
        stepPosition = CTP();

        forW = Vector3.zero;

        DefaultPlaceOnCamera.transform.position = gameObject.transform.position + new Vector3(3f,1f,0f);
    }

    private void Update()
    {
        inJump |= Input.GetButtonDown("Jump");

        float mouseX = Input.GetAxis("Mouse X") * mouseSpeed;
        float mouseY = Input.GetAxis("Mouse Y") * mouseSpeed;
        plainRotateX -= mouseX;
        plainRotateY -= mouseY;

        LockValue();

        Vector3 cameraTargetPoint = CTP();

        Vector2 playerInput;
        playerInput.y = Input.GetAxis("Horizontal");
        playerInput.x = Input.GetAxis("Vertical");

        if (onGround == true || airMove)
        {
            playerInput = Vector2.ClampMagnitude(playerInput, 1f);
        }

        Vector3 directionCross = Vector3.Cross(forW, -Physics.gravity).normalized;
        desiredVelocity = (playerInput.x * Vector3.Cross(-Physics.gravity, directionCross).normalized - playerInput.y * directionCross) * maxSpeed;
        forW = cameraTargetPoint - cam.transform.position;

        RotationPuzzle(cameraTargetPoint);
    }

    private void LockValue()
    {
        if (plainRotateX > 180f)
        {
            plainRotateX -= 360f;
        }
        else if (plainRotateX < -180f)
        {
            plainRotateX += 360f;
        }

        if (plainRotateY > 89f)
        {
            plainRotateY = 89f;
        }
        else if (plainRotateY < -89f)
        {
            plainRotateY = -89f;
        }
    }

    private void RotationPuzzle(Vector3 cameraTargetPoint)
    {
        //腳色前後一幀的移動位移，加在相機位置上
        DefaultPlaceOnCamera.transform.position += cameraTargetPoint - stepPosition;
        stepPosition = cameraTargetPoint;

        //重力前後一幀的旋轉角度，RotateAround到真正的DefaultPlaceOnCamera
        float angleChange1 = Vector3.Angle(stepRotrte, Physics.gravity);
        DefaultPlaceOnCamera.transform.RotateAround(cameraTargetPoint, Vector3.Cross(rig.velocity, -Physics.gravity).normalized, angleChange1);
        
        //旋轉DefaultPlaceOnCamera的位置
        cam.transform.position = DefaultPlaceOnCamera.transform.position;
        cam.transform.RotateAround(cameraTargetPoint, Physics.gravity, plainRotateX);
        cam.transform.RotateAround(cameraTargetPoint, Vector3.Cross(cam.transform.position-cameraTargetPoint, -Physics.gravity), plainRotateY);

        //相機的碰撞
        RaycastHit hit;
        if (Physics.Raycast(cameraTargetPoint, cam.transform.position-cameraTargetPoint, out hit,LayerMask.NameToLayer("Ground")))
        {
            if(hit.collider != null)
            {
                if (Vector3.Distance(hit.point, cameraTargetPoint)<=Mathf.Sqrt(9f))
                {
                    cam.transform.position = hit.point + (cameraTargetPoint - hit.point).normalized * 0.2f;
                }
            }
        }
    }

    private void LateUpdate()
    {
        Vector3 cameraTargetPoint = CTP();
        
        cam.transform.LookAt(cameraTargetPoint, -Physics.gravity);
        stepRotrte = Physics.gravity;
    }

    // Update is called once per frame
    void FixedUpdate()
    {
        Vector3 toGravity = (Vector3.Dot(rig.velocity, Physics.gravity) / Physics.gravity.magnitude) * Physics.gravity.normalized;

        velocity = rig.velocity - toGravity;

        float maxSpeedChange = mixAccelerate * Time.deltaTime;
        if (onGround == true || airMove)
        {
            velocity = Vector3.MoveTowards(velocity, desiredVelocity, maxSpeedChange);
        }




        velocity += toGravity;

        if (inJump == true)
        {
            if (onGround == true)
            {
                float jumpSpeed = Mathf.Sqrt(2 * jumpHight * Physics.gravity.magnitude);
                velocity += -Physics.gravity.normalized * jumpSpeed;
            }
            inJump = false;
        }

        rig.velocity = velocity;


        Vector3 w = Vector3.zero;
        for(int i=0;i<TargetGravity.Length;i++)
        {
            w += gameObject.transform.position - TargetGravity[i].transform.position;
        }
        Physics.gravity = w.normalized * w.magnitude/3;

        Vector3 upAxis = -Physics.gravity.normalized;

        onGround = false;
    }

    private void OnCollisionEnter(Collision collision)
    {
        Collide(collision);
    }
    private void OnCollisionStay(Collision collision)
    {
        Collide(collision);
    }

    private void Collide(Collision collision)
    {
        for(int i =0; i< collision.contactCount; i++)
        {
            Vector3 normal = collision.GetContact(i).normal;
            onGround |= Vector3.Angle(-Physics.gravity, normal) <= jumpAngleLimet;
        }
    }
}
