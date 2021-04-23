#include "GenericBlob.h"
#include <kApi/Io/kMemory.h>
#include <kApi/Io/kSerializer.h>
#include <kApi/Data/kArray1.h>


BlobProperty::BlobProperty() :
    _area{ k64F_NULL },
    _aspect{ k64F_NULL },
    _centerHeight{ k64F_NULL },
    _angle{ k64F_NULL },
    _center{ k32F_NULL,k32F_NULL },
    _centerHoriz{ k32F_NULL,k32F_NULL },
    _centerContourPts{ k32F_NULL,k32F_NULL },
    _width{ k64F_NULL },
    _length{ k64F_NULL },
    _contourPtCount{ kSIZE_NULL },
    _minAreaRectCorners{ { { k32F_NULL,k32F_NULL }, { k32F_NULL,k32F_NULL }, { k32F_NULL,k32F_NULL }, { k32F_NULL,k32F_NULL } } },
    _boundingBox{ k32S_NULL,k32S_NULL,k32S_NULL,k32S_NULL }
{
    _pContourPoints = new std::vector<kPoint64f>();
}

BlobProperty::~BlobProperty()
{
    delete _pContourPoints;
}

// Copy
BlobProperty::BlobProperty(const BlobProperty& other) :
    _area{ other._area },
    _aspect{ other._aspect },
    _centerHeight{ other._centerHeight },
    _angle{ other._angle },
    _center{ other._center.x, other._center.y },
    _centerHoriz{ other._centerHoriz.x, other._centerHoriz.y },
    _centerContourPts{ other._centerContourPts.x, other._centerContourPts.y },
    _width{ other._width },
    _length{ other._length },
    _contourPtCount{ other._contourPtCount },
    _minAreaRectCorners{ { { other._minAreaRectCorners[0].x,other._minAreaRectCorners[0].y },
                            { other._minAreaRectCorners[1].x,other._minAreaRectCorners[1].y }, 
                            { other._minAreaRectCorners[2].x,other._minAreaRectCorners[2].y },
                            { other._minAreaRectCorners[3].x,other._minAreaRectCorners[3].y } } },
    _boundingBox{ other._boundingBox.x, other._boundingBox.y, other._boundingBox.width, other._boundingBox.height }
{
    if (other._pContourPoints != nullptr)
    {
        _pContourPoints = new std::vector<kPoint64f>();
        *_pContourPoints = *other._pContourPoints;
    }
    else
    {
        _pContourPoints = nullptr;
    }
}

BlobProperty& BlobProperty::operator=(const BlobProperty& other)
{
    if (this != &other)
    {
        _area = other._area;
        _aspect = other._aspect;
        _centerHeight = other._centerHeight;
        _angle = other._angle;
        _center = other._center;
        _centerHoriz = other._centerHoriz;
        _centerContourPts = other._centerContourPts;
        _width = other._width;
        _length = other._length;
        _contourPtCount = other._contourPtCount;
        _minAreaRectCorners = other._minAreaRectCorners;
        _boundingBox = other._boundingBox;

        if (other._pContourPoints != nullptr)
        {
            if (_pContourPoints == nullptr)
            {
                _pContourPoints = new std::vector<kPoint64f>();
            }
            *_pContourPoints = *other._pContourPoints;
        }
        else
        {
            if (_pContourPoints != nullptr)
            {
                delete _pContourPoints;
                _pContourPoints = nullptr;
            }
        }
    }
    return *this;
}

// Move
BlobProperty::BlobProperty(BlobProperty&& other) noexcept :
    _area{ other._area },
    _aspect{ other._aspect },
    _centerHeight{ other._centerHeight },
    _angle{ other._angle },
    _center{ other._center.x, other._center.y },
    _centerHoriz{ other._centerHoriz.x, other._centerHoriz.y },
    _centerContourPts{ other._centerContourPts.x, other._centerContourPts.y },
    _width{ other._width },
    _length{ other._length },
    _contourPtCount{ other._contourPtCount },
    _minAreaRectCorners{ { { other._minAreaRectCorners[0].x,other._minAreaRectCorners[0].y },
                            { other._minAreaRectCorners[1].x,other._minAreaRectCorners[1].y },
                            { other._minAreaRectCorners[2].x,other._minAreaRectCorners[2].y },
                            { other._minAreaRectCorners[3].x,other._minAreaRectCorners[3].y } } },
    _boundingBox{ other._boundingBox.x, other._boundingBox.y, other._boundingBox.width, other._boundingBox.height }
{
    _pContourPoints = other._pContourPoints;
    other._pContourPoints = nullptr;
}

BlobProperty& BlobProperty::operator=(BlobProperty&& other)
{
    if (this != &other)
    {
        _area = other._area;
        _aspect = other._aspect;
        _centerHeight = other._centerHeight;
        _angle = other._angle;
        _center = other._center;
        _centerHoriz = other._centerHoriz;
        _centerContourPts = other._centerContourPts;
        _width = other._width;
        _length = other._length;
        _contourPtCount = other._contourPtCount;
        _minAreaRectCorners = other._minAreaRectCorners;
        _boundingBox = other._boundingBox;

        if (_pContourPoints) delete _pContourPoints;

        _pContourPoints = other._pContourPoints;
        other._pContourPoints = nullptr;
    }
    return *this;
}

// Getter and Setter
k64f BlobProperty::GetArea() const { return _area; }
void BlobProperty::SetArea(k64f area) { _area = area; }

k64f BlobProperty::GetAspect() const { return _aspect; }
void BlobProperty::SetAspect(k64f aspect) { _aspect = aspect; }

k64f BlobProperty::GetCenterHeight() const { return _centerHeight; }
void BlobProperty::SetCenterHeight(k64f height) { _centerHeight = height; }

k64f BlobProperty::GetAngle() const { return _angle; }
void BlobProperty::SetAngle(k64f angle) { _angle = angle; }

const kPoint32f& BlobProperty::GetCenter() const { return _center; }
void BlobProperty::SetCenter(const kPoint32f& pt) { _center = pt; }

const kPoint32f& BlobProperty::GetCenterHoriz() const { return _centerHoriz; }
void BlobProperty::SetCenterHoriz(const kPoint32f& pt) { _centerHoriz = pt; }

const kPoint32f& BlobProperty::GetCenterContourPts() const { return _centerContourPts; }
void BlobProperty::SetCenterContourPts(const kPoint32f& pt) { _centerContourPts = pt; }

k64f BlobProperty::GetWidth()const { return _width; }
void BlobProperty::SetWidth(k64f width) { _width = width; }

k64f BlobProperty::GetLength() const { return _length; }
void BlobProperty::SetLength(k64f length) { _length = length; }

kSize BlobProperty::GetContourPtCount() const { return _contourPtCount; }
void BlobProperty::SetContourPtCount(kSize count) { _contourPtCount = count; }

const std::array<kPoint32f, 4>& BlobProperty::GetMinAreaRectCorners() const { return _minAreaRectCorners; }
void BlobProperty::SetMinAreaRectCorners(const std::array<kPoint32f, 4>& minAreaRectCorners) { _minAreaRectCorners = minAreaRectCorners; }

const kRect32s& BlobProperty::GetBoundingBox() const { return _boundingBox; }
void BlobProperty::SetBoundingBox(const kRect32s& rect) { _boundingBox = rect; }

const std::vector<kPoint64f>& BlobProperty::GetContourPoints() const { return *_pContourPoints; }
void BlobProperty::SetContourPoints(const std::vector<kPoint64f>& contourPoints) { *_pContourPoints = contourPoints; }


// Serialize and Deserialize
kStatus SerializeBlob(const BlobProperty* input, kSerializer serializer)
{
    k32u version = 1;
    kCheck(kSerializer_Write32u(serializer, version));

    kCheck(kSerializer_Write64f(serializer, input->GetArea()));

    kCheck(kSerializer_Write64f(serializer, input->GetAspect()));
    kCheck(kSerializer_Write64f(serializer, input->GetCenterHeight()));
    kCheck(kSerializer_Write64f(serializer, input->GetAngle()));

    kCheck(kSerializer_Write32f(serializer, input->GetCenter().x));
    kCheck(kSerializer_Write32f(serializer, input->GetCenter().y));
    kCheck(kSerializer_Write32f(serializer, input->GetCenterHoriz().x));
    kCheck(kSerializer_Write32f(serializer, input->GetCenterHoriz().y));
    kCheck(kSerializer_Write32f(serializer, input->GetCenterContourPts().x));
    kCheck(kSerializer_Write32f(serializer, input->GetCenterContourPts().y));

    kCheck(kSerializer_Write64f(serializer, input->GetWidth()));
    kCheck(kSerializer_Write64f(serializer, input->GetLength()));
    kCheck(kSerializer_WriteSize(serializer, input->GetContourPtCount()));

    for (const auto& corner : input->GetMinAreaRectCorners())
    {
        kCheck(kSerializer_Write32f(serializer, corner.x));
        kCheck(kSerializer_Write32f(serializer, corner.y));
    }

    kCheck(kSerializer_Write32s(serializer, input->GetBoundingBox().x));
    kCheck(kSerializer_Write32s(serializer, input->GetBoundingBox().y));
    kCheck(kSerializer_Write32s(serializer, input->GetBoundingBox().width));
    kCheck(kSerializer_Write32s(serializer, input->GetBoundingBox().height));

    kCheck(kSerializer_WriteSize(serializer, (kSize)(input->GetContourPoints().size())));

    for (const auto& point : input->GetContourPoints())
    {
        kCheck(kSerializer_Write64f(serializer, point.x));
        kCheck(kSerializer_Write64f(serializer, point.y));
    }

    return kOK;
}

kStatus DeserializeBlob(BlobProperty* output, kSerializer serializer)
{
    // Increase version if a change occurs in the struct, and implement a different reader for each version.
    k32u version;
    kSize ctrPointCount;

    kCheck(kSerializer_Read32u(serializer, &version));

    switch (version)
    {
    default:
        return kERROR;

    case 1:
        k64f area;
        kCheck(kSerializer_Read64f(serializer, &area));
        output->SetArea(area);

        k64f aspect;
        kCheck(kSerializer_Read64f(serializer, &aspect));
        output->SetAspect(aspect);

        k64f centerHeight;
        kCheck(kSerializer_Read64f(serializer, &centerHeight));
        output->SetCenterHeight(centerHeight);

        k64f angle;
        kCheck(kSerializer_Read64f(serializer, &angle));
        output->SetAngle(angle);

        kPoint32f center;
        kCheck(kSerializer_Read32f(serializer, &center.x));
        kCheck(kSerializer_Read32f(serializer, &center.y));
        output->SetCenter(center);

        kPoint32f centerHoriz;
        kCheck(kSerializer_Read32f(serializer, &centerHoriz.x));
        kCheck(kSerializer_Read32f(serializer, &centerHoriz.y));
        output->SetCenterHoriz(centerHoriz);

        kPoint32f centerContourPts;
        kCheck(kSerializer_Read32f(serializer, &centerContourPts.x));
        kCheck(kSerializer_Read32f(serializer, &centerContourPts.y));
        output->SetCenterContourPts(centerContourPts);

        k64f width;
        kCheck(kSerializer_Read64f(serializer, &width));
        output->SetWidth(width);

        k64f length;
        kCheck(kSerializer_Read64f(serializer, &length));
        output->SetLength(length);

        kSize contourPtCount;
        kCheck(kSerializer_ReadSize(serializer, &contourPtCount));
        output->SetContourPtCount(contourPtCount);

        std::array<kPoint32f, 4> minAreaRectCorners;
        for (auto& corner : minAreaRectCorners)
        {
            kCheck(kSerializer_Read32f(serializer, &corner.x));
            kCheck(kSerializer_Read32f(serializer, &corner.y));
        }
        output->SetMinAreaRectCorners(minAreaRectCorners);

        kRect32s boundingBox;
        kCheck(kSerializer_Read32s(serializer, &boundingBox.x));
        kCheck(kSerializer_Read32s(serializer, &boundingBox.y));
        kCheck(kSerializer_Read32s(serializer, &boundingBox.width));
        kCheck(kSerializer_Read32s(serializer, &boundingBox.height));
        output->SetBoundingBox(boundingBox);

        kCheck(kSerializer_ReadSize(serializer, &ctrPointCount));
        std::vector<kPoint64f> contourPoints;
        contourPoints.reserve(ctrPointCount);
        for (kSize i = 0; i < ctrPointCount; ++i)
        {
            kPoint64f point;
            kCheck(kSerializer_Read64f(serializer, &point.x));
            kCheck(kSerializer_Read64f(serializer, &point.y));
            contourPoints.push_back(point);
        }
        output->SetContourPoints(contourPoints);

        break;
    }

    return kOK;
}

kExportDx(kStatus) SerializeBlobs(const std::vector<BlobProperty>& blobs, kArray1* outputBuffer)
{
    kMemory memory = kNULL;
    kSerializer serializer = kNULL;
    kStatus exception = kOK;

    k8u* out;
    k8u* in;
    kSize len;

    kTry
    {
        if (!blobs.empty() && outputBuffer != kNULL)
        {

            kMemory_Construct(&memory, kNULL);
            kSerializer_Construct(&serializer, memory, kNULL, kNULL);

            kTest(kSerializer_WriteSize(serializer, blobs.size()));

            for (auto& blob : blobs)
            {
                kTest(SerializeBlob(&blob, serializer));
            }

            kTest(kSerializer_Flush(serializer));

            len = (kSize)kMemory_Length(memory);
            kTest(kArray1_Construct(outputBuffer, kTypeOf(k8u), len, kNULL));
            out = (k8u*)kArray1_Data(*outputBuffer);
            in = (k8u*)kMemory_At(memory, 0);
            kTest(kMemCopy(out, in, len));
        }
    }
    kCatchEx(&exception)
    {
        kEndCatchEx(exception);
    }
    kFinallyEx
    {
        kDestroyRef(&memory);
        kDestroyRef(&serializer);

        kEndFinallyEx();
    }

    return kOK;
}

kExportDx(kStatus) DeserializeBlobs(std::vector<BlobProperty>& blobs, void * inputBuffer, kSize length, kAlloc alloc)
{
    kSize count = 0;
    kSerializer serializer = kNULL;
    kMemory memory = kNULL;
    kStatus exception = kOK;

    kTry
    {
        kTest(kMemory_Construct(&memory, alloc));
        kTest(kMemory_Attach(memory, inputBuffer, 0, length, length));

        kTest(kSerializer_Construct(&serializer, memory, kNULL, alloc));
        kTest(kSerializer_ReadSize(serializer, &count));

        for (kSize i = 0; i < count; ++i)
        {
            BlobProperty output;
            kTest(DeserializeBlob(&output, serializer));
            blobs.push_back(std::move(output));
        }
    }
    kCatchEx(&exception)
    {
        kEndCatchEx(exception);
    }
    kFinallyEx
    {
        kDestroyRef(&serializer);
        kDestroyRef(&memory);

        kEndFinallyEx();
    }

    return kOK;
}

#if !defined(GDK_HELPER_LIBRARY_EMIT) && !defined(GDKAPP)

kBool GoGenericMsgTypeValid(GoGenericMsg genMsg)
{
    if (genMsg)
    {
        k32u utype = GoGenericMsg_UserType(genMsg);
        if (utype == GENERIC_BLOB_DATA_TYPE)
        {
            return kTRUE;
        }
    }

    return kFALSE;
}

kStatus DeserializeBlobs(GoGenericMsg genMsg, std::vector<BlobProperty>& blobs)
{
    if (GoGenericMsgTypeValid(genMsg))
    {
        void* ptr = (void*)GoGenericMsg_BufferData(genMsg);
        DeserializeBlobs(blobs, ptr, GoGenericMsg_BufferSize(genMsg), kNULL);

        return kOK;
    }

    return kERROR;
}

#endif
