
#import "TornEffect.h"
#import "ActiveXDialog.h"


TornEffect::TornEffect(void) //ª˝º∫¿⁄ 
{
	m_pNodedata = NULL;
}


TornEffect::~TornEffect(void) // º“∏Í¿⁄
{
	Destroy();
}


void TornEffect::Destroy()
{
	if(m_pNodedata){
		m_pNodedata->Destroy();
		delete m_pNodedata;
		m_pNodedata = NULL;
	}
}

//ªˆ¡æ¿Ã øµø™¿« TornEffect(ªˆ¡æ¿Ã ¡∂∞¢¿« TornEffect(∏¬¥›¥¬ ∫Œ∫–)¿« Invers«œø© »ø∞˙ ¿˚øÎ) 
TornEffect* TornEffect::GetInvers() 
{
	TornEffect * ret = new TornEffect();
	if(!m_pNodedata ){
		return ret;
	}

	ret->m_pNodedata = getInversNode(m_pNodedata);
	return ret;
}

MNode* TornEffect::getInversNode(MNode* node)
{
	if(!node)
		return NULL;

	MNode* ret;
	ret = new MNode();
	MFRACTAL* retval = new MFRACTAL;
	retval->mid = 1 - ((MFRACTAL *)node->data)->mid ;			// ¡ﬂ¡°	
	retval->distance = ((MFRACTAL *)node->data)->distance * -1;	// ≥Ù¿Ã
	ret->data = retval;


	int n = node->child.size();
	if(n != 0){
		ret->child.resize(n);
		for(int i = 0; i < n ; i++){
			ret->child[i] = getInversNode( node->child[ roundRoutine(i+1, n) ] );
		}
	}

	return ret;	
}

//«¡∑∫≈ª ≥ÎµÂ ª˝º∫
void TornEffect::InitEffect(float ratio, int depth)
{
	if( m_pNodedata )
		m_pNodedata->Destroy();
	m_pNodedata = getFractal(ratio, depth, 1.0f);
}

MNode* TornEffect::getFractal(float ratio, int depth, float length)
{
	if(length <= 0.05)
		return NULL;
	MNode* ret;
//	srand(time(NULL));
	int randmid = random( 41 ) + 30;
	int randdis = random( int((ratio*200) +1) ) - ratio*100;

	MFRACTAL* retval = new MFRACTAL;
	retval->mid = randmid/100.0;
	retval->distance = randdis/100.0;
	ret = new MNode();
	ret->data = retval;
	if(depth > 1){
		ret->child.resize(2);

		ret->child[0] = getFractal(ratio, depth-1, length*retval->mid);
		ret->child[1] = getFractal(ratio, depth-1, length*(1-retval->mid));
	}

	return ret;	
}

// ¥Ÿ∞¢«¸ ø°¡ˆ¿« µŒ ∫§≈Õ ªÁ¿Ã¿« «¡∑∫≈ª ∆˜¿Œ∆Æ ∏Æ≈œ
vector< MPoint > TornEffect::GetTornEffectPoint(MPoint a, MPoint b, int minDis, float ratio)
{
	vector< MPoint > ret;

	ret = getFractalPoint(a, b, m_pNodedata, minDis, ratio);

	return ret;
}

vector< MPoint > TornEffect::getFractalPoint(MPoint a, MPoint b, MNode* info, int minDis, float ratio)
{
	vector< MPoint > ret;
	if(!info){
		ret.push_back(a);
	}else{
		double dis = sqrt( double((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y)) );
		if(dis <= minDis)
			ret.push_back(a);
		else{
			// calculate rand fractal applied mid point
			double randmid, randdis;
			randmid = ((MFRACTAL *)info->data)->mid;
			randdis = ((MFRACTAL *)info->data)->distance * dis;

			MPoint mid;
			mid.x = a.x*randmid + b.x*(1-randmid);
			mid.y = a.y*randmid + b.y*(1-randmid);

			double gx, gy;
			double temp;

			gx = b.y - a.y;
			gy = -(b.x - a.x);
			temp = gx*gx + gy*gy;
			temp =sqrt(temp);

			gx = gx/temp;
			gy = gy/temp;

			mid.x = mid.x + gx*randdis*ratio;
			mid.y = mid.y + gy*randdis*ratio;

			// process next depth, and summate result
			if(info->child.size() == 0){
				ret.push_back(a);
				ret.push_back(mid);
			}else{
				vector< MPoint > retb;
				ret = getFractalPoint(a,mid,info->child[0],minDis,ratio);
				retb = getFractalPoint(mid,b,info->child[1],minDis,ratio);
				for( int i = 0; i < (int)retb.size(); i++){
					ret.push_back(retb[i]);
				}
				retb.clear();
			}
		}
	}
	return ret;
}

// «¡∑∫≈ª ≥ÎµÂ ¿Øπ´ ∆«¥‹
bool TornEffect::hasEffect()
{
	if(m_pNodedata)
		return true;
	else
		return false;
}